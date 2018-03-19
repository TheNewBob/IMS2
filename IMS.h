#pragma once

//forward declaration of GUI classes; 
//necessary because otherwise a whole lot of files have to include all GUI headers without actually needing them.
class GUImanager;
class GUI_Surface;
class GUI_MainDisplay;

class IMS_Module;
class IMS_Orbiter_ModuleData; 
class IMS_General_ModuleData;
class IMS_Component_Model_Base;
class EventGenerator;
class EventSink;
class Event_Base;
class EventHandler;
class IMS_CoGmanager;
class IMS_PropulsionManager;
class IMS_RcsManager;
class IMS_TouchdownPointManager;

class IMS_Manager_Base;
class IMS_Autopilot_Base;


enum IMS_MANAGER;
enum AP_MODE;

//panel identifiers
const int FLIGHTPANEL = 0;
const int ENGINEERINGPANEL = 1;

struct LOCATION_CONTEXT;


//data struct used to exchange splitting information between a vessel and its newly created offspring
struct SPLIT_VESSEL_DATA
{
	vector<IMS_Module*> modules;
	IMSATTACHMENTPOINT *originattachment = NULL;
	int newcoreattachmentindex = -1;
	MATRIX3 transform;
	bool split = false;
	
	void clear()
	{
		modules.clear();
		modules.shrink_to_fit();
		originattachment = NULL;
		newcoreattachmentindex = -1;
		split = false;
	}
};


class IMS2 : public VESSEL4, public EventHandler
{

public:
	IMS2(OBJHANDLE hVessel, int flightmodel);
	~IMS2();

	
	//IMS.cpp
	void clbkPreStep(double simt, double simdt, double mjd); 
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	int clbkNavProcess(int mode);
	void clbkNavMode(int  mode, bool  active);

	UINT AddNewMesh(std::string meshName, VECTOR3 pos);

	void AssimilateMe(vector<IMS_Module*> &modulesToAssimilate);
	void AssimilateIMSVessels(vector<std::string> &vesList);
	void Split(vector<IMS_Module*> modules_to_remove);
	void RemoveModule(IMS_Module *module);


	//IMS_State.cpp
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkPostCreation(); 
	void clbkSaveState (FILEHANDLE scn);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetStateEx(const void *  status);


	//IMSgetSet.cpp
	VISHANDLE GetVisHandle();
	GUImanager *GetGUI();
	IMSATTACHMENTPOINT *GetConnectingAttachmentPoint(VESSEL *_vessel);
	IMSATTACHMENTPOINT *IMS2::GetConnectingAttachmentPointByStatus(OBJHANDLE dockedv);
	UINT GetDockIndex(IMSATTACHMENTPOINT *ap);
	UINT GetDockIndex(DOCKHANDLE dockH, VESSEL *vessel);
	void SetForRedock();
	void GetStack(vector<VESSEL*> &stackList, OBJHANDLE callingVessel = NULL);
	void GetModules(vector<IMS_Module*> &OUT_modules);
	IMS_Module *GetCoreModule(){ return modules[0]; };
	vector<DOCKEDVESSEL*> &GetDockedVesselsList();

	/**
	 * \return all components that can currently be added to the vessel.
	 * \todo Currently a stub that returns all compnents. Change when implementing a cargo and inventory system.
	 */
	vector<IMS_Component_Model_Base*> &GetAddableComponents(vector<LOCATION_CONTEXT> &contexts);

	//manager getters
	IMS_PropulsionManager *GetPropulsionManager();
	IMS_CoGmanager *GetCoGmanager();
	IMS_RcsManager *GetRcsManager();
	IMS_TouchdownPointManager *GetTdPointManager();

	/**
	 * \brief Adds an animation to the vessel
	 *
	 * Essentially wrapps VESSEL::CreateAnimation, but checks whether there are
	 * deleted animations on the vessel that can be replaced to avoid having unused
	 * animation objects lying around.
	 * The whole function is based on, and in fact necessitated by, the fact that Orbiter never
	 * truly deletes an animation, merely its components. The "corpse" of the animation structure is 
	 * kept in memory to not upset the indexing of the other animations. Such an invalid structure can be re-used
	 * by a new animation without problems.
	 * \param initial_state The animation state corresponding to the unmodified mesh
	 * \return Animation identifier
	 * \note This function relies on you to add animation components to a created animation before creating a new animation,
	 *	since an animation without components is considered dead and will be overwritten.
	 */
	UINT CreateAnim(double initial_state);

	
	/* returns the center of gravity relative to the core module
	*/
	VECTOR3 GetCoG();

	/* returns a position in CoG relative (Orbiter vessel local) coordinates
	 * from a position relative to the vessels core module (IMS vessel internal coordinates)
	 */
	VECTOR3 GetPositionRelativeToCoG(VECTOR3 pos);

	/*returns whether the vessel is landed or not right now
	*/
	bool GetLandedState() { return islanded; };


	//IMSPanel.cpp
	bool clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	bool clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf, void *context);
	bool clbkPanelMouseEvent (int id, int event, int mx, int my, void *context);
	

	//IMS_Docking.cpp
	//DOCKEDVESSEL *UpdateDockedVesselsList(VESSEL *vessel, IMSATTACHMENTPOINT *point, bool remove = false, bool dealloc = true);
	DOCKEDVESSEL *RegisterDockedVessel(VESSEL *vessel, IMSATTACHMENTPOINT *point);
	void UnregisterDockedVessel(VESSEL *vessel, IMSATTACHMENTPOINT *point, bool dealloc = true);
	void clbkDockEvent(int dock, OBJHANDLE mate);
	void ConnectAttachmentPoints();


	static SURFHANDLE pilotPanelBG;		//background pilot panel
	static SURFHANDLE engPanelBG;		//background engineering panel
	static SPLIT_VESSEL_DATA SplitVesselData;

	//IMS_VesselEvents.cpp
	void RegisterEventSinkWithGenerator(EventSink *moduleeventsink);
	bool UnregisterEventSinkWithGenerator(EventSink *moduleeventsink);
	void RegisterEventGeneratorWithSink(EventGenerator *moduleeventgenerator);
	bool UnregisterEventGeneratorWithSink(EventGenerator *moduleeventgenerator);


private:
	//IMS.cpp

	/**
	* \brief is called at the end of every clbkPreStep on the vessel.
	* This essentially serves the purpose to process the loopback pipe
	* of the eventhandler. It is executed after everybody has done its thing
	* and sent all its messages, and is used to finalise the vessel state
	* before Orbiter applies it.It should NOT be overriden by inheriting classes.
	* If you push events to the waiting queue, this is the time they will fire,
	* and you get to finalise your states by reacting to them.
	*/
	void preStateUpdate();	
	void AssimilateIMSVessel(IMS2 *vessel);

	/**
	 * \brief checks things in the vessel state and triggers appropriate events when something changes
	 * triggerevents: pass false to just update the state without triggering events (only done on vessel creation)
	 */
	void updateVesselState(bool triggerevents = true);

	//IMS_Docking.cpp
	void CreateDockedVesselsList();
	void UpdateDockPorts(bool hasAssimilated = false);
	void HandleDockEvent();
	
	/* returns a pointer to the DOCKEDVESSEL struct wrapping the passed VESSEL pointer.
	 * returns NULL if vessel is not in this vessel's docking list.
	 */
	DOCKEDVESSEL *GetDockedVessel(VESSEL *vessel);

	//IMSState.cpp
	bool AddNewModule(IMS_Orbiter_ModuleData *orbiter_data, IMS_General_ModuleData *module_data);
	bool LoadModuleFromScenario(FILEHANDLE scn, bool initFromCfg);

	/**
	 * \brief Calls in order: PostLoad() on modules, AddModuleToVessel() on modules, PostLoad() on managers.
	 * Is called at the end of either clbkLoadStateEx() or during clbkSetStateEx(), whichever is applicable.
	 * \param created_from_split If true, PostLoad() on modules will not be called, as the modules are not actually loaded in this case.
	 */
	void postLoad(bool created_from_split);

	//IMS_Panel.cpp
	void DefineMainPanel(PANELHANDLE hPanel);
	void DefineEngPanel(PANELHANDLE hPanel, DWORD width, DWORD height);
	void SetPanelScale (PANELHANDLE hPanel, DWORD viewW, DWORD viewH);
	void InitialiseGUI();				//!< initialises GUI surfaces so their headers don't have to be included in any other files
	void DestroyGUI();
	//calls the GUI update. Only exists so the whole GUI
	//doesn't have to be included in IMS.cpp
	void updateGui();



	//IMS_GetSet.cpp
	VESSELTYPE GetVesselType(VESSEL* _vessel);
	DOCKHANDLE GetDockByStatus(OBJHANDLE dockedv);
	UINT GetDockIndex(DOCKHANDLE dockH);


//	MESHHANDLE hPanelMesh;                       // 2-D instrument panel mesh handle
 	GUImanager *GUI;							 //GUI manager that handles GUI for this vessel
	//panel elements
 	GUI_Surface *mainDispSurface;


	vector<IMS_Module*> modules;
	VISHANDLE hVis = NULL;										 //VISHANDLE of the whole vessel
	vector<DOCKEDVESSEL*> dockedVesselsList;

	//misc
	bool isSetForRedock;						//true if the vessel has integrated. On vessel deletion while docked a dockevent is called, manipulating the docking port in the time between can have weird consequences
	bool islanded;								//true if the vessel is landed
	map<IMS_MANAGER, IMS_Manager_Base*> managers;		//!< stores the various manager instances of this vessel
	map<AP_MODE, IMS_Autopilot_Base*> autopilots;		//!< stores autopilot instances.
	
	//EventGenerator and -Sink that communicate with the IMS_Module instances of this vessel
	EventGenerator *eventgenerator;
	EventSink *eventsink;


	//EventHandler overload
	bool ProcessEvent(Event_Base *e);

};	


/**********************************************************

Orbiter EventCue on integration (2010 P1, not guaranteed for 2015!):
v1: vessel calling integration
v2: vessel being integrated

V1 clbkMouseEvent->triggerRedraw->Assimilate->delete v2
V1 clbkPanelRedrawEvent
V1 clbkPreStep
v2 clbkPreStep
v1 clbkPostStep
v2 clbkPostStep
v1 clbkDockEvent->deldock
v1 clbkPanelRedrawEvent (triggered from dockevent - suboptimal)
v2 clbkDockEvent
v2 ~v2
v1 clbkPreStep
v1 clbkPostStep
************************************************************/