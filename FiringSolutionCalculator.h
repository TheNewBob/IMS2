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
	 * \brief Applies the firing solution to the vessels thrusters to achieve the demanded torque and force vectors.
	 * \note Has no effect if called on an instance that is still calculating its solution.
	 * \see IsSolutionReady()
	 */
	void Apply(VECTOR3 &torque, VECTOR3 &force, double thrust);

	/**
	 * \return true if the solution has been calculated, false if it is still running.
	 */
	bool IsSolutionReady() { return solutionready; };

private:
	void calculateFiringSolutions();

	FIRING_SOLUTION constructFiringSolution(VECTOR3 &desiredForce, VECTOR3 &desiredTorque);

//	FIRING_SOLUTION scaleFiringSolution(const FIRING_SOLUTION& sol);

//	FIRING_SOLUTION simplifyFiringSolution(const FIRING_SOLUTION& sol);

	FIRING_SOLUTION calculateFiringSolution(THGROUP_TYPE group);

	/**
	 * \return A firing solution with the passed coefficients, and all other thrusters initialised to 0
	 * \param sol A firing solution that contains all thrusters actually contributing to the solution
	 */
	FIRING_SOLUTION completeFiringSolution(FIRING_SOLUTION &sol);

//	void firingSolutionResult(const FIRING_SOLUTION& sol, VECTOR3& force, VECTOR3& torque);

	map<FORCETYPE, VECTOR3> calculateGeneratedForce(FIRING_SOLUTION &sol, vector<FiringSolutionThruster*> &involvedthrusters);

	/**
	 * \brief returns the principle directional vector a thruster group symbolises
	 */
	VECTOR3 getPrincipleTorqueVectorForGroup(THGROUP_TYPE group);
	VECTOR3 getPrincipleForceVectorForGroup(THGROUP_TYPE group);

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

};

