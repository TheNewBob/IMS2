#pragma once


class IMS_Orbiter_ModuleData;
class IMS_General_ModuleData;
class IMS_ModuleFunction_Base;
class GUIplugin;
class IMS_Movable;

//the id for the module's GUI page. Since there will only ever be one element 
//with that ID per plugin, this will not cause conflicts. 
const int GUI_MODULE_PAGE = INT_MAX;		


struct VesselToDock				//used for storing information on a vessel that should be docked to a module. Only used on loading.
{
	UINT portIndex;
	std::string vesselName;
};



 /**
 * \brief A physical component of a spacecraft that can be added or removed at runtime
 *
 * The Module class is the heart of IMS2: It manages Attachment points and Mesh placement and
 * transformation, and contains one or several ModuleFunction objects as required to adapt
 * to the demanded functionality. However, while Modules can be added or removed to a vessel
 * at will, a module instance itself is static in terms of capability for the duration of its
 * lifetime, as defined by a config file.
 * A Module is never destroyed, unless the containing vessel is destroyed. If a Module changes ownership,
 * its overall state and the overall state of its functions are unaffected.
 * 
 * A Module is also an EventHandler that provides a throughput eventpipe from the vessel to the modulefunctions.
 * The EventPipe back from a modulefunction however is passed to all other module functions INSIDE THIS MODULE,
 * and are only passed down to the vessel if the module takes action to do so.
 */
class IMS_Module : public EventHandler
{
public:
	friend class GUI_Module;

	IMS_Module();
	virtual ~IMS_Module();

	//functions that propagate to IMS_ModuleFunction objects
	
	/**
	 * \brief initialises the Module with static data from a config file
	 * @param orbiter_data The orbiter api specific data for the module (like meshname, size etc)
	 * @param module_data The general data for the module, like mass, module functions etc.
	 * @param mVessel The vessel that will initially contain this module
	 */
	virtual bool InitModule(IMS_Orbiter_ModuleData *orbiter_data, IMS_General_ModuleData *module_data, IMS2 *mVessel);
	
	/**
	 * \brief A callback that is called when all data has been loaded, including data from a scenario state.
	 * \note This function interfaces to IMS_ModuleFunction_Base
	 * @see IMS_ModuleFunction_Base::PostLoad()
	 */
	virtual void PostLoad();

	/**
	 * \brief Called at every clbkPreStep() of the containing vessel
	 * @param simdt The time elapsed between this frame and the previous one
	 * \note This function interfaces to IMS_ModuleFunction_Base
	 * @see IMS_ModuleFunction_Base::PreStep()
	 */
	virtual void PreStep(double simdt);
	
	/**
	* \brief is called at the end of every clbkPreStep on the vessel.
	* This essentially serves the purpose to process the loopback pipe
	* of the eventhandler. It is executed after everybody has done its thing
	* and sent all its messages, and is used to finalise the vessel state
	* before Orbiter applies it.It should NOT be overriden by inheriting classes.
	* If you push events to the waiting queue, this is the time they will fire,
	* and you get to finalise your states by reacting to them.
	*/
	void PreStateUpdate();

	/** 
	 * \brief called from IMS2::clbkSaveState() to write the module state to scenario file
	 * @param scn The FILEHANDLE to the Scenario file being written
	 * @param index The current index of this module in the vessel. This is only needed as an identifier in the Scenario.
	 * \note This function interfaces to IMS_ModuleFunction_Base
	 * @see IMS_ModuleFunction_Base::SaveState()
	 */
	virtual void SaveState(FILEHANDLE scn, UINT index);

	/**
	 * \brief loads the modules saved state from scenario file
	 * @param scn the FILEHANDLE to the scenario file being loaded
	 * \note This function interfaces to IMS_ModuleFUnction_Base
	 * @see IMS_ModuleFUnction_Base::LoadState()
	 */
	virtual bool LoadState(FILEHANDLE scn);
	
	/**
	* \brief Called when this module is removed from a vessel.
	*
	* overload for module types that register things or need to modify the base vessel
	* overloaded methods should ALWAYS invoke the base method because of attachment point
	* and mesh handling!
	* \note This function interfaces to IMS_ModuleFunction_Base
	* @see IMS_ModuleFunction_Base::RemoveFunctionFromVessel()
	*/
	virtual void RemoveModuleFromVessel();
	
	/**
	* \brief Performs all actions necessary when adding the module to a vessel.
	* \note The module must at this point be fully initialised, including
	* its ModuleFunctions and its vessel properties!
	* @param _vessel The vessel the module is added to
	* @param rotatemesh Whether the mesh needs to be newly transformed
	* @param addMesh whether the mesh needs to be added
	* \note	rotatemesh must only be true if module was a member of another vessel before, otherwise this happens automatically when the visual is created 
	* \note addmesh must always be true except if this is the first module in a vessel, which has its mesh loaded by orbiter itself
	*/
	virtual void AddModuleToVessel(IMS2 *_vessel, bool rotatemesh = false, bool addMesh = true);
	
	/**
	 * \brief Returns the combined mass of the module and all its functions/components.
	 */
	virtual double GetMass();

	/**
	 * \brief Returns a rotation matrix to transform from vessel-relative to the module-relative frame.
	 */
	MATRIX3 GetRotationMatrix() { return rotationmatrix; };

	/**
	 * \brief Returns the modules position in orbiter local (vessel relative) coordinates.
	*/
	VECTOR3 GetLocalCoordinates();

	/**
	 * \brief Returns the index the modules mesh has in the vessel.
	 */
	UINT GetMeshIndex() { return meshIndex; };

	/**
	 * \brief Returns the vessel containing this module
	 */
	IMS2 *GetVessel()  { return vessel; };

	/**
	 * \brief Returns a string describing the module by name and position.
	 */
	string getModulePosDescription();

	/**
	 * \brief Removes the module's mesh from the vessel
	 */
	virtual void RemoveMeshFromVessel();

	/**
	 * \brief Spawns a new vessel that has the same position, orientation and class as this module.
	 * @param _name The name for the newly created vessel
	 * @param velocity Direction and magnitude of velocity relative to separating vessel
	 */
	void SpawnNewVesselFromModule(string _name, VECTOR3 velocity);

	/**
	 * \brief Rotates the module's mesh into the correct position relative to the vessel
	 */
	void RotateMesh();

	/**
	 * \brief Fills the passed reference with the module's rotational vector
	 * @param mRot Reference to receive the rotation ("up"-direction)
	 */
	void GetRot(VECTOR3 &mRot);

	/**
	* \brief Fills the passed reference with the module's directional vector
	* @param mDir Reference to receive the direction ("front"-direction)
	*/
	void GetDir(VECTOR3 &mDir);

	/**
	* \brief Fills the passed reference with the module's position
	* @param mPos Reference to receive the position
	*/
	void GetPos(VECTOR3 &mPos);

	/**
	 * \brief Returns a pointer to the module's general static data
	 */
	IMS_General_ModuleData *GetGeneralModuleData(){ return _module_data; };
	
	/**
	 * Sets the vessel-relative rotation of the module
	 * @param mRot The desired rotational vector ("up"-direction, normalised)
	 */
	void SetRot(VECTOR3 &mRot);

	/**
	 * \brief Sets the vessel-relative direction of the module
	 * @param mDir The desired directional vector ("front"-direction)
	 */
	void SetDir(VECTOR3 &mDir);

	/**
	* \brief Sets the vessel-relative position of the module
	* @param mPos The desired position
	*/
	void SetPos(VECTOR3 &mPos);
	
	//IMS_BaseModule_AttachmentPoints.cpp

	/**
	 * \brief Fills the passed std::vector with the module's list of attachment points
	 * @param OUT_points: Reference to std::vector to receive the list. Will be overwritten!
	 * @see AddAttachmentPointsToList()
	 */
	void GetAttachmentPoints(vector<IMSATTACHMENTPOINT*> &OUT_points);

	/**
	 * \brief Appends this module's attachment points to the passed std::vector
	 * @param IN_OUT_points An std::vector to receive the attachment points. Existing data will not be changed.
	 * \note use this function instead of GetAttachmentPoints() if you need to collect the attachment points of
	 *	several modules in the same list, it's much faster.
	 * @see GetAttachmentPoints()
	 */
	void AddAttachmentPointsToList(vector<IMSATTACHMENTPOINT*> &IN_OUT_points);
	
	/**
	 * \brief Returns the attachment point connecting to the passed vessel
	 * @param _v the vessel for which you need to know to which attachment point on this module it is docked
	 * \ return A pointer to an IMSATTACHMENTPOINT object, or NULL if the vessel is not docked to this module
	 */
	IMSATTACHMENTPOINT *GetConnectingAttachmentPoint(VESSEL *_v);

	/**
	 * \brief returns the attachment point that connects to the passed module 
	 * @param module The module for which you need to know to which attachment point on this module it is connected
	 * \return e, A pointer to an IMSATTACHMENTPOINT object, or NULL if the module is not docked to this module
	 */
	IMSATTACHMENTPOINT *IMS_Module::GetConnectingAttachmentPoint(IMS_Module* module);

	
	/**
	 * \brief Returns the index of an attachment point on this module.
	 * @param point A pointer to the attachment point you need to know the index of
	 * \return The index of the attachment point, or -1 if point is not located on this module
	 * \note The index in the module is the only attribute of an attachment point that's guaranteed to be consistent over sessions!
     */
	int GetAttachmentPointIndex(IMSATTACHMENTPOINT *point);
	
	/**
	 * \brief Updates state of attachment points and associated dockports as well as vessels docked to those ports.
	 * @param redock pass true if vessels formerly docked to this attachment point should be redocked instead of unregistered as docked vessels.
	 * \note vessels are necessarily undocked at events that chenge the structure of the vessel (i.e. adding or removing modules).
	 *	This is the only situation in which redock sould be true
	 */
	void UpdateAttachmentPoints(bool redock = false);

	/**
	 * \brief Returns the modules principle moments of inertia ("inertia" parameter in cfg) in the passed VECTOR3.
	 */
	void GetPmi(VECTOR3 &OUT_pmi);
	/**
	 * \brief creates the actual physical attachment points of this vessel (i.e. the orbiter dockports).
	 * \note This is an initialiser function and only called at very specific places in the code
	 */
	void CreatePhysicalAttachmentPoints();

	/**
	 * \brief Transforms attachment points to face the right way and be in the right place.
	 */
	void TransformAttachmentPoints();

	/**
	 * \brief Gets called after assimilating a module, because all DOCKHANDLEs in the module are now invalid
	 * \todo with proper event handling, it might be possible to make this function private
	 */
	void SetDockPortsToNull();

	/**
	 * \brief rotates an attachment point around its directional axis
	 * @param att Pointer to the attachment point to be rotated
	 * @param angle The rotational angle in radians
	 */
	void RotateAttachmentPoint(IMSATTACHMENTPOINT *att, double angle);

	/**
	 * \brief disconnects attachment points and deletes their docking ports.
	 *
	 * This does NOT delete the attachment points themselves, merely their physical representation
	 * in Orbiter. it will also remove all DOCKEDVESSEL objects from the vessel without deallocating them. 
	 * They remain in memory, and the attachment point stays connected to them, and will be redocked 
	 * this MODULE if UpdateAttachmentPoints() is called with redock true. This enables the transfer
	 * of docked vessels from one vessel to another in the event of a split.
	 */
	void RemoveAllAttachmentPoints();

	/**
	 * \brief transforms the module to preserve position and orientation relative to the passed module in its origin state.
	 *
	 * A module in its origin state means that its position/orientation is 0, 0, 1 / 0, 1, 0. This module is transformed so it
	 * has the same position and orientation relative to the passed module that it has now, after the other module is rotated
	 * from iuts current orientation to origin. This is used to preserve the relative orientation of modules in a subset of 
	 * the vessel that is being split off.
	 * @param module A pointer to the module this module should keep its relative position and orientation to.
	*/
	void SetOrientationRelativeTo(IMS_Module *module);

	/**
	 * \brief Returns the GUIplugin of this module, NULL if it doesn't have a GUI.
	 */
	GUIplugin *GetGui() { return guiplugin; };


	//Comparators, in IMS_ModuleComparators.cpp

	/**
	 * \brief Comparator to sort modules by their position along the X axis
	 */
	static bool X_ORDER(IMS_Module *a, IMS_Module *b);

	/**
	* \brief Comparator to sort modules by their position along the Y axis
	*/
	static bool Y_ORDER(IMS_Module *a, IMS_Module *b);

	/**
	* \brief Comparator to sort modules by their position along the Z axis
	*/
	static bool Z_ORDER(IMS_Module *a, IMS_Module *b);

	/**
	* \brief Comparator to sort modules by their symmetric position along the X axis
	*/
	static bool X_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b);

	/**
	* \brief Comparator to sort modules by their symmetric position along the Y axis
	*/
	static bool Y_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b);

	/**
	* \brief Comparator to sort modules by their symmetric position along the Z axis
	*/
	static bool Z_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b);

	//technically, this is for comparing attachment points, not modules, but the two are unseparable anyways
	/**
	* \brief Comparator to sort attachment points by their overall position.
	*
	* Sorting with this comparator has the effect that overlapping attachment points will be next 
	* to each other in the resulting list.
	*/
	static bool ATTACHMENT_POINTS_BY_POS(IMSATTACHMENTPOINT *a, IMSATTACHMENTPOINT *b);

protected:
	vector<IMS_ModuleFunction_Base*> functions;			//!< A list containing all this module's ModuleFunction objects

	/**
	 * \brief Resets attachment points to their original state
	 *
	 * The original state is defined as the state of the attachment point on the newly created,
	 * unintegrated module. The state includes position, rotation, direction as well as connection
	 * state and dockport state
	 */
	void resetAttachmentPoints();
	
	/**
	 * \brief Saves attributes common to all modules
	 * \param scn FILEHNDLE to the open scenario file to write to
	*/
	void SaveBaseAttributes(FILEHANDLE scn);
	
	/**
	 * \brief Loads attributes common to all modules from a scenario file
	 * \param scn FILEHANDLE of the scenario file to read from
	 */
	bool LoadScenarioBaseAttributes(FILEHANDLE scn);

	/**
	 * \brief Adds the module's mesh to the meshes list of the vessel
	 * \param rotateMesh Whether the mesh needs to be newly transformed
	 * \note rotateMesh Must only be true if module was a member of another vessel before, otherwise this happens automatically when the visual is created
	 * \see AddModuleToVessel()
	 */
	bool AddMeshToVessel(bool rotateMesh = false);

	IMS2 *vessel;										//!< The vessel containing this module

	IMS_Orbiter_ModuleData *_orbiter_data;				//!< Orbiter related attributes of this module
	IMS_General_ModuleData *_module_data;				//!< General attributes of this module
	UINT moduleIndex;									//!< Index of the module in the modules list
	UINT meshIndex;										//!< Index of the module mesh in the vessel
	VECTOR3 pos;										//!< The module's current position in vessel-relative coordinates
	VECTOR3 rot;										//!< The module's current orientation ("up"-direction), vessel-relative
	VECTOR3 dir;										//!< The module's current directional vector, vessel-relative
	MATRIX3 rotationmatrix;								//!< Matrix for transformations into the module-relative frame

	vector<IMSATTACHMENTPOINT*> attachmentPoints;		//!< Vector containing all the attachment points of this module
	
	/**
	* \brief docks a vessel wraped by a DOCKEDVESSEL struct to the intended attachment point on this module
	* \param v The DOCKEDVESSEL that you want docked
	* \param p The attachment point v needs to dock to
	* \note If you already have a DOCKEDVESSEL structure, it means the vessel was docked to this module
	*	and has been undocked during the last frame. Re-docking it with this function insures that the
	*	vessel will be redocked to the same attachment point on the same module, no matter whether the
	*	module still is part of the same vessel or if the attachment point still wraps the same DOCKHANDLE.
	*/
	void dockVessel(DOCKEDVESSEL *v, IMSATTACHMENTPOINT *p);	
	
	
	virtual bool ProcessEvent(Event_Base *e);
	
	/**
	 * \brief creates the module gui.
	 * Only called once in the lifetime of a module
	 */
	virtual void createGui();

	GUIplugin *guiplugin = NULL;				//!< pointer to the module's GUI. If NULL, this module doesn't have a GUI
};