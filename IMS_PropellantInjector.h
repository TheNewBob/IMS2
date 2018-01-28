#pragma once

class IMS_ModuleFunction_Thruster;

/**
 * \brief Handles the proliferation of propellant mixtures to thrusters
 *
 * You should not think of this as a physical piece of machinery running in the ship.
 * This is a purely virtual construct for the benefit of easier code, it does not have any physical
 * equivalent. It is named PropellantInjector because the job-description comes close to what it does
 * (proliferation of correct mixtures from tanks to thrusters), but it really only exists because orbiter
 * insists that a thruster take its propellant from a single propellant resource.
 */
class IMS_PropellantInjector 
{
public:
	/**
	 * \param propellant_ids A list of all propellant ids contained in this injectors mixture
	 * \param ratio a list of ratioes at which the corresponding ids have to be mixed.
	 * \param prop_efficiency The efficiency at which this mixture is converted into thrust by the thruster
	 * \param vessel The vessel this PropellantInjector is located on
	 * \note Exaple: propellant_ids{1,2} and ratio{1,6} means that the propellant type with id 1
	 *	and the propellant type with id 2 have to be mixed at a ratio of 1:6.
	 * \notepropellant_ids and ratio must have the same size!
	 */
	IMS_PropellantInjector(vector<int> &propellant_ids, vector<float> &ratio, double efficiency, VESSEL *vessel);
	~IMS_PropellantInjector();

	/**
	 * \brief Connects a tank to this injector.
	 * \param tank The IMS_Storable to be added as a tank
	 * \return True if the tank was added, false if the tank did not contain any consumable compatible with this injector
	 * \see DisconnectTank()
	 */
	bool ConnectTank(IMS_Storable* tank);

	/**
	* \brief Disconnects a tank from this injector.
	* \param tank The IMS_Storable to be removed from the injector
	* \return True if the tank was disconnected, false if the tank was not actually connected to this injector
	* \see ConnectTank()
	*/
	bool DisconnectTank(IMS_Storable* tank);
	
	/**
	 * \brief Connects a thruster to this injector
	 * \param thruster The handle to a orbiter thruster
	 * \param maxmassflow The maximum massflow of the thruster, in kg/s
	 * \note In contrast to ConnectTank() and DisconnectTank(), this method has no way of verifying
	 *	if the passed thruster is actually suited for this injector. Make sure to check compatibility 
	 *	using CompareMixture() before adding a thruster to an injector!
	 * \see DisconnectThruster()
	 */
	void ConnectThruster(THRUSTER_HANDLE thruster, double maxmassflow);

	/**
	 * \brief Removes a thruster from this injector.
	 * \param thruster the handle to an orbiter thruster
	 * \param maxmassflow the maximum massflow of the thruster, in kg/s
	 * \return True if the thruster was disconnected, false if the thruster was not actually connected to this injector
	 * \note This should only be called if a thruster is removed from a vessel.
	 *	The injector will disconnect its internal propellant resource
	 *	from its thrusters when the tanks run dry to disable the thruster, this
	 *	method must not be called to do that.
	 * \see ConnectThruster
	 */
	bool DisconnectThruster(THRUSTER_HANDLE thruster, double maxmassflow);

	/**
	 * \brief Marks a thruster as ready for operation
	 * \param thruster An orbiter THRUSTER_HANDLE to the thruster to be enabled
	 * \return True if the thruster is now enabled (regardless of whether or not it was before),
	 *	false if the thruster isn't actually connected to this injector.
	 */
	bool EnableThruster(THRUSTER_HANDLE thruster);

	/**
	 * \brief Marks a thruster as not currently operational
	 * \param thruster An orbiter THRUSTER_HANDLE to the thruster to be disabled
	 * \return True if the thruster is now disabled (regardless of whether or not it was before),
	 *	false if the thruster isn't actually connected to this injector.
	 */
	bool DisableThruster(THRUSTER_HANDLE thruster);

	/**
	 * \return The number of thrusters this injector currently connects to
	 */
	int GetNumberOfThrusters() { return thrusters.size(); };

	double GetMass() { return injectormass; };
	
	/**
	 * \brief Calculates propellant consumption since last call.
	 * 
	 * Resizes virtual propellant resource if necessary, draws consumed propellant from connected tanks
	 * \param simdt Elapsed sim time since last frame.
	 * \return true if the mass of the virtual propellant resource has changed, false otherwise.
	 */
	bool PreStep(double simdt);
	
	/**
	 * \return True if the mixture of this injector matches the mixture passed in the arguments, false otherwise
	 */
	bool CompareMixture(vector<int> &propellant_ids, vector<float> &ratio, double efficiency);

	/**
	 * \return the total amount of propellant available to the injector, in kg
	 * \note This does not take the mixture into account! It may well be that the injector
	 *	has a ton of propellant available, but can only burn 100 kg because the propellants 
	 *	are not proportional to the ratio at which they are consumed!
	 * \see GetMaximumConsumablePropellantMass()
	 */
	double GetAvailablePropellantMass();

	/**
  	 * \return The maximum mass of propellant that can actually be consumed.
	 * This takes ratio into account, so if you have e.g. 500 kg of fuel and 500 kg of oxydiser,
	 * But your mixture requires twice the amount of fuel per oxydiser, the returned value will be
	 * 750 kg (500 kg of fuel plus 250 kg of oxydiser).
	 */
	double GetMaximumConsumablePropellantMass();

	/**
	 * \brief Fills up the dummy propellant resources at simulation start, because Orbiter will have reset them before clbkPostCreation.
	 */
	void FillInjectorDummy();

private:
	map<int, vector<IMS_Storable*>> tanks;					//!< contains all currently connected tanks 
	map<int, double> ratio;							//!< maps ratioes to their respective propellant types 
	map<THRUSTER_HANDLE, bool> thrusters;			//!< Stores all handles to thrusters proliferated by this injector, and whether they are enabled or not
	PROPELLANT_HANDLE injector = NULL;				//!< The orbiter propellant resource the injector uses to satisfy orbiter. It also serves as a kind of feedback event to know how much propellant is actually consumed.

	double currenttimewarp;							//!< the time acceleration the sim is currently in. Needed for scaling the propellant resource.
	double totalmaxmassflow = 0;					//!< the maximum total massflow that can pass through this injector, in kg/s
	VESSEL *v;
	double injectormass = 1;
	bool injectormasschanged = false;				//!< True if the mass of the virtual resource has changed during this frame
	bool isViable = false;							//!< Shows whether the injector has all necessary propellants to actually work
	
	/**
	 * \brief Checks whether the injector can work with the currently connected tanks and sets the isViable flag accordingly.
	 */
	void checkValidity();				

	/**
	 * \brief Disables all thrusters in the injector, but without actually changing their enabled status.
	 * This is used when the injector doesn't have enough propellant anymore to work. The operational state of
	 */
	void disableInjector();

	/**
	 * \brief Enables all thrusters that are enabled.
	 * This is used to re-enable an injector when new propellant becomes available. The operational status of individual thrusters is not affected.
	 */
	void enableInjector();

	/**
	 * Scales the virtual propellant resource depending on time acceleration and mximum massflow of the engine
	 * \note This method changes the mass of the underlying orbiter vessel. Any caller is responsible to insure that the vessel mass  
	 *	of the IMS vessel is recalculated in the same frame, but should take care that it is not done twice in the same frame.
	 */
	void scaleInjectorResource(double simdt);

	/**
	 * \return True if any connected thrusters are running, false otherwise.
	 */
	bool areThrustersRunning();

	/**
	 * \brief Kills the thrust on all thrusters attached.
	 * This is used to prevent asymetric thruster shutdown by orbiter when there's not
	 * enough propellant around to feed them for another frame.
	 */
	vector<THRUSTER_HANDLE> getRunningThrusters();

};