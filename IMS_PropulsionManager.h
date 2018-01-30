#pragma once

class IMS_Storable;
class IMS_PropellantInjector;


/**
 * \brief Manages Storables available for propellant consumption (i.e. propellant tanks), as well as thrusters.
 *
 * It's important to realise that the purpose of this manager is to offer Storables SPECIFICALLY for use as propellant tanks
 * This manager should always have knowledge of all Storables used as propellant tanks currently on the vessel.
 * Storables registered and opened with this manager will be used when a thruster requires their consumables as propellant, 
 * and closed Storables cannot be used by thrusters, even if they would have the right contents.
 *
 * \note The class is slightly misnamed, as it also turned out to kinda manage thrusters.
 * \see IMS_Storable
 */
class IMS_PropulsionManager : 
	public IMS_Manager_Base
{
public:
	/**
	 * \param vessel The VESSEL instance this PropellantManager is responsibly for
	 */
	IMS_PropulsionManager(IMS2 *_vessel);
	~IMS_PropulsionManager();

   /**
	* \brief Adds a new tank to the propellant manager
	* \param tank A storable to be added as tank
	* \note Do not add the same tank 2 times
	* \note If a tank isn't added to the propellant manager, it effectively doesn't exist on the vessel, even if the module is there.
	*/
	void AddTank(IMS_Storable *tank);
	
   /**
	* \brief Removes a tank from the propellant manager
	* \param tank The tank to be removed from the manager
	* \note A tank not unregistered from the propellant manager will still be controlled by this manager, even if the module is not in this vessel anymore!
	*/
	void RemoveTank(IMS_Storable *tank);
	
	/**
	 * \brief Opens a tank to make it accessible to thrusters
	 * \param tank The tank to be opened. It is expected that the tank has been added 
	 * \param forceevaluation Pass true to force injectors to evaluate the tank, even if it was already open.
	 * \note Usually the tank will only be evaluated by injectors if the valve was closed before,
	 *	since it can be assumed that otherwise it would already be registered with the propper injectors.
	 *	This does not hold true if a tank is added to the vessel with valve already open, in which case this method
	 *	must be forced to pass the tank to the injectors for evaluation in any case.
	 *	to the PropellantManager prior to this operation!
	 */
	void OpenTank(IMS_Storable *tank);

	/**
	 * \brief Closes a tank and makes it inaccessible to thrusters
	 * \param tank The tank to be closed. It is expected that the tank has been added
	 *	to the PropellantManager prior to this operation!
	 */
	void CloseTank(IMS_Storable *tank);

    /**
     * \brief Gets or creates an Injector compatible with the passed mixture
     * \param propellant_ids A list of all ids of propellants appearing in the mixture
     * \param ratio A list with the mix ratios for the above propellants
     * \param efficiency The efficiency of the mixture, between 0 and 1
	 * \param exhausts A list of exhaust data that defines all exhausts associated with the thruster, pos and dir vessel-relative!
     * \return A pointer to an Injector compatible with the propellant mixture, either pre-existing or newly created.
     * \note This method guarantees that there is only one injector for each mixture in the manager.
     * \note ratios to be given in whole numbers, sequentially matched with propellant_ids.
     * example: propellant_ids {1, 2} and ratio {2, 3} means propellants 1 and 2 are mixed in a ratio of 2 to 3.
     */
	THRUSTER_HANDLE AddThruster(THRUSTERMODE *thrustermode, VECTOR3 &pos, VECTOR3 &dir);

   /**
	* \brief Removes a thruster from the manager and/or from the orbiter vessel
	* \param thruster A handle to the thruster to be removed
	* \param maxflowrate The maximum flowrate of the thruster (needed to adjust size of virtual propellant source)
	* \param deletefromvessel Pass false if you do not want the thruster handle deleted from the vessel
	* \note Thruster MUST be removed from any groups prior to calling this, or there will be trouble!
	*/
	void RemoveThruster(THRUSTER_HANDLE thruster, double maxflowrate, bool deletefromvessel = true);

	/**
	 * \brief Changes the thrustermode of an already added thruster.
	 * \param thruster The thruster handle of the thruster that switches modes.
	 * \param mode The thrustermode thruster is to be set to.
	 */
	void SwitchThrusterMode(THRUSTER_HANDLE thruster, THRUSTERMODE *mode);

   /**
	* \brief Enables or disables a thruster. A disabled thruster is considered not to be in operational condition at this time by the manager.
	* \param thruster A handle to the thruster to be enabled or disabled.
	* \param enabled Pass true if the thruster is ready to operate, false if not.
	*/
	void SetThrusterEnabled(THRUSTER_HANDLE thruster, bool enabled);
	
	/**
	 * \brief Adds a thruster to a thruster group.
	 * \param thruster Handle to the thruster to be added.
	 * \param group The thruster group the thruster should be added to
	 */
	void AddThrusterToGroup(THRUSTER_HANDLE thruster, THGROUP_TYPE group);

	/**
	 * \brief Removes a thruster from a thruster group
	 * \param thruster The thruster to be removed from a group
	 * \param group The group the thruster is currently a member of
	 */
	void RemoveThrusterFromGroup(THRUSTER_HANDLE thruster, THGROUP_TYPE group);

	/**
	* \brief adds exhausts to a thruster
	* \param thruster The thruster the new exhausts are to be associated with
	* \param mode The thrustermode set for that thruster
	* \param exhausts creation data for the exhausts
	* \note You have to call this method separately from creating a thruster. The reason is that PropellantManager
	*	cannot keep track of the relative position of the module, and there are only methods in Orbiter to create
	*	and delete exhausts, not to change them. So when an exhaust changes, Propellantmanager has to delete the old one,
	*	but then doesn't have the information to judge where the new has to be placed.
	*/
	void AddExhausts(THRUSTER_HANDLE thruster, vector<THRUSTEREXHAUST> &exhausts);

	/**
	* \brief Removes all exhausts associated with a thruster
	* \param The thruster for which to delete the exhausts
	* \note If you have exhausts that change, call this first and then add them again.
	*/
	void RemoveExhausts(THRUSTER_HANDLE thruster);


   /**
	* \brief Performs the prestep update of the PropellantManager and all injectors, thrusters and tanks it is aware of.
	*/
	void PreStep(double simdt);

private:
	map<int, vector<IMS_Storable*>> tanks;
	vector<IMS_PropellantInjector*> injectors;							//!< list of currently present injectors
	map<THRUSTER_HANDLE, vector<UINT>> liveexhausts;			//!< Maps exhaust definitions to a thruster handle
	map<THGROUP_TYPE, vector<THRUSTER_HANDLE>> thgroups;				//!< Contains the list of thrusters for each group

	bool ProcessEvent(Event_Base *e);

	/**
	 * \brief Creates an IMS_PropellantInjector in the PropellantManager if a compatible one doesn't already exist
	 * \param ratio a list of ratioes at which the corresponding ids have to be mixed.
	 * \param prop_efficiency The efficiency at which this mixture is converted into thrust by the thruster
	 * \param vessel The vessel this PropellantInjector is located on
	 * \return The newly created injector, or the existing injector compatible with the mixture
	 * \see IMS_PropellantInjector
	 */
	IMS_PropellantInjector *getInjector(vector<int> propellant_ids, vector<float> ratio, double efficiency);

	/**
	 * \brief Updates a thruster group on the orbiter vessel
	 * \param group The group to update
	 * \note This MUST be called after any changes to thgroups, as the array wrapped
	 *	by a vector is not guaranteed to retain its memory address when changing!
	 */
	void updateThrusterGroup(THGROUP_TYPE group);

	/**
	 * \brief Adds a tank to all injectors that
	 */
	void addTankToInjectors(IMS_Storable *tank);
	
	/**
	 * \brief Removes a tank from all injectors
	 */
	void removeTankFromInjectors(IMS_Storable *tank);
};

