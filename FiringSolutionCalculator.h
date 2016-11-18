#pragma once

struct FIRING_SOLUTION
{
	std::map<THRUSTER_HANDLE, double> coefficents;
	FIRING_SOLUTION & operator*=(double rhs);
	const FIRING_SOLUTION operator*(double rhs);
	FIRING_SOLUTION & operator+=(const FIRING_SOLUTION& rhs);
};

struct THRUSTERPROPERTIES
{
	THRUSTERPROPERTIES(THRUSTER_HANDLE _handle, double _score)
	{
		handle = _handle;
		score = _score;
	}
	THRUSTER_HANDLE handle;
	double score;
};

enum FORCETYPE;

class FiringSolutionThruster;
/**
 * \brief Calculates a new firing solution for Rcs thrusters in a new thread.
 *
 * Instantiating the class immediately starts a new thread in which the new solution is calculated.
 * Use the IsSolutionReady() method to querry if the calculation has finished.
 * The thread can be safely terminated at any point by deleting the instance. The thread is joined in the destructor.
 * The thread is safe under most circumstances, except for when one of the RCS thruster actually get deleted from the
 * vessel. Make sure to kill the thread first, then delete any thrusters.
 * \author meson800
 */
class FiringSolutionCalculator
{
public:
	FiringSolutionCalculator(IMS2 *vessel, vector<THRUSTER_HANDLE> rcsthrusters);
	~FiringSolutionCalculator();

	/**
	 * \brief Applies the firing solution to the vessels thrusters to achieve the demanded torque and force levels.
	 * \note Has no effect if called on an instance that is still calculating its solution.
	 * \param torque Vector containing the thrust levels for rotation groups in the three axes, between -1.0 and 1.0.
	 * \param force Vector containing the thrust levels for translation groups in the three axes, between -1.0 and 1.0.
	 * \note A level of 1.0 or -1.0 means that the maximum thrust will be applied that still guarantees avoiding artifacts.
	 *	If both torque and force are requested at the same time, that caution is thrown to the wind.
	 * \see IsSolutionReady()
	 */
	void Apply(VECTOR3 &torque, VECTOR3 &force, double thrust);

	/**
	* \brief Calculates the thrust levels to achieve the requested torque. 
	* \param torque Desired torque around 3 axes, in Nm. Will be converted to values between -1.0 and 1.0, proportional to the maximum maneuverability.
	* \note If the torque cannot be produced, hrusters will be set to max level allowed to avoid lateral force.
	*/
	void CalculateTorqueLevels(VECTOR3 &IN_OUT_torque);

	/**
	 * \return true if the solution has been calculated, false if it is still running.
	 */
	bool IsSolutionReady() { return solutionready; };


private:
	void calculateFiringSolutions();

	FIRING_SOLUTION constructFiringSolution(VECTOR3 &desiredForce, VECTOR3 &desiredTorque);

	FIRING_SOLUTION calculateFiringSolution(THGROUP_TYPE group);

	/**
	 * \return A firing solution with the passed coefficients, and all other thrusters initialised to 0
	 * \param sol A firing solution that contains all thrusters actually contributing to the solution
	 */
	FIRING_SOLUTION completeFiringSolution(FIRING_SOLUTION &sol);

	/**
	 * \brief Calculates the total force a firing solution can produce with a certain subset of thrusters.
	 * \param sol The firing solution you want to know the generated forces for.
	 * \param involvedthrusters A list of all thrusters to be used in the solution.
	 * \return A map with two vectors keyed to F_LINEAR and F_TORQUE.
	 */
	map<FORCETYPE, VECTOR3> calculateGeneratedForce(FIRING_SOLUTION &sol, vector<FiringSolutionThruster*> &involvedthrusters);

	/**
	 * \brief Notes the passed forces as the maximum forces this group can produce.
	 * \param group The thruster group the forces are for.
	 * \param forces a map containing the maximum values for F_TORQUE and F_LINEAR.
	 * \note Since this saves magnitudes, the forces are always positive. Howver, the sign of the forces
	 *	you pass to this method do not matter, it will convert them itself.
	 */
	void setMaxForcesForGroup(THGROUP_TYPE group, map<FORCETYPE, VECTOR3> forces);

	/**
	 * \return the thruster groups able to generate a certain force
	 * \param force The force vector you are looking to generate
	 * \param type Whether force represents linear force or torque
	 */
	vector<THGROUP_TYPE> getGroupsFromForceVector(VECTOR3 &force, FORCETYPE type);

	/**
	* The current firing solutions for the various actions rcs has to perform
	*/
	FIRING_SOLUTION pitchUpSol, pitchDownSol, yawRightSol, yawLeftSol, bankRightSol, bankLeftSol
		, leftSol, rightSol, upSol, downSol, forwardSol, backSol;

	FSThrusterCollection *thrusters = NULL;

	vector<THRUSTER_HANDLE> thrusterhandles;
	IMS2 *vessel;
	bool solutionready = false;						//!< shows true if the solution is calculated
	bool abort = false;								//!< flag that signifies the thread to terminate at the next opportunity.
	thread *calculationthread;
	map<THGROUP_TYPE, map<FORCETYPE, VECTOR3>> maxforces;
};

