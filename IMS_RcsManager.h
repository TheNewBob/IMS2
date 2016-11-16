#pragma once

class FiringSolutionCalculator;

/**
 * \brief Manages and controls RCS thrusters.
 *
 * Rcs behavior is controlled according to a pretty complex solution that tries to get induce as little secondary effects
 * as possible, no matter where the thrusters are located. If your design is extremely bad this might simply not be possible,
 * but in general it works pretty nicely.
 * This way of doing things requires that the rcs thrusters actually not be part of a thruster group. The levels of the 
 * individual thrusters are controlled directly. This in turn means that there have to be virtual thrusters created
 * to fill up the standard rotation and translation thruster groups, as it would otherwise be impossible to read 
 * the user input to these thrusters.
 * \author meson800, Jedidia
 */
class IMS_RcsManager :
	public IMS_Manager_Base
{
public:
	IMS_RcsManager(IMS2 *_vessel);
	~IMS_RcsManager();

	/**
	 * \brief Adds a thruster to the rcs managment
	 * \param thruster The thruster to add as an rcs thruster
	 * \note This does not create any thrusters on the vessel. The thruster
	 *	itself is expected to be created and managed by the PropulsionManager.
	 *	RcsManager merely notes that the thruster is used as rcs (yes, technically
	 *	ANY old thruster could be added here, doesn't make a difference!)
	 * \see AddThrusterPair(), RemoveThruster()
	 */
	void AddThruster(THRUSTER_HANDLE thruster);

	/**
	* \brief Adds a thruster as a pair to the rcs managment.
	* \param thruster1 one of the thruster to add as an rcs thruster.
	* \param thruster2 Another thruster to add, its direction being opposite of thruster1.
	* \see AddThruster(), RemoveThruster()
	*/
	void AddThrusterPair(THRUSTER_HANDLE thruster1, THRUSTER_HANDLE thruster2);

	/**
 	 * \brief Removes a thruster to the rcs managment
	 * \param thruster The thruster to remove (must have been added prior!)
	 * \note This does not delete any thrusters on the vessel. The thruster merely
	 *	stops being used as an RCS thruster.
	 * \note If the thruster is part of a pair, both thrusters will be removed!
	 * \see AddThruster(), AddThrusterPair()
	 */
	void RemoveThruster(THRUSTER_HANDLE thruster);


	/**
	 * \brief Checks if the firing solution is still valid and recalculates if necessary
	 * \param simdt Time elapsed sice last frame
	 */
	void PreStep(double simdt);


	/**
	 * \brief Enables or disables the intelligent RCS.
	 * The option to disable it is for compatibility with MFD autopilots that rely on querying RCS performance.
	 * \param enabled
	 */
	void SetIntelligentRcs(bool enabled);

	/**
	 * \return True if RCS is in intelligent mode, false otherwise.
	 */
	bool GetIntelligentRcs() { return intelligentrcs; };

	/**
	 * Sets commanded torque for next frame.
	 * Use this to control intelligent RCS with autopiliots.
	 * \note User input will still override this setting!
	 */
	void SetCommandedTorque(VECTOR3 &torque);

private:
	bool ProcessEvent(Event_Base *e);

	/**
	 * \brief Creates the virtual rcs needed to process userinput
	 */
	void createDummyThrusters();

	/**
	* \brief Destroys the virtual rcs
	*/
	void destroyDummyThrusters();

	/**
	 * \brief Adds the vessels RCS thrusters to the actual rcs groups, 
	 *	enabling direct control by autopilots but losing its intelligence.
	 */
	void createPhysicalRcsGroups();

	/**
	* \brief Destroys the unintelligent RCS groups, but leaves the thrusters untouched.
	*/
	void destroyPhysicalRcsGroups();

	/**
	 * \brief reads userinput and converts it into a force vector
	 */
	void getCommandedForce(VECTOR3 &OUT_force);

	/**
	* \brief reads userinput and converts it into a torque vector
	*/
	void getCommandedTorque(VECTOR3 &OUT_torque);

	/**
	 * \brief starts the calculation of a new firing solution in a new thread.
	 * \note The new solution will be allocated as newfiringsolution.
	 */
	void calculateFiringSolution();

	/**
	 * \brief aborts calculation of new firing solution and deletes the current one.
	 */
	void destroyFiringSolution();



	THRUSTER_HANDLE dummyThrusters[24];						//!< array of virtual thrusters to gauge the applied thrust. Unfortunately the thruster group level will not return correctly if there are no thrusters in the group.
	vector<THRUSTER_HANDLE> rcsthrusters;					//!< an array containing all thruster handles that are registered as RCS.
	PROPELLANT_HANDLE dummypropellant;						//!< virtual propellant source for the virtual thrusters.

	bool dummiesexist = false;								//!< flag indicating whether the dummy thrusters currently exist.
	bool thrustersfiring = false;							//!< Flag to easily track whether thrusters are currently firing.
	bool intelligentrcs = true;								//!< registers if RCS is set to intelligent mode
	FiringSolutionCalculator *firingsolution = NULL;		//!< The firing solution currently in use.
	FiringSolutionCalculator *newfiringsolution = NULL;		//!< When allocated, means that a new solution is being calculated in a separate thread.
	VECTOR3 commandedtorque = _V(0, 0, 0);					//!< If non-user input sets a torque, it will be remembered here until processed.
	bool torqueset = false;									//!< True if torque was commanded during the current frame.
};

