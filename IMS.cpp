#define ORBITER_MODULE
#include "Common.h"
#include "Managers.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "Calc.h"
#include "IMS_Manager_Base.h"
#include "IMS_CoGmanager.h"
#include "IMS_PropulsionManager.h"
#include "IMS_RcsManager.h"
#include "SimpleShape.h"
#include "IMS_TouchdownPointManager.h"
#include "autopilot_includes.h"
#include "GUI_Common.h"
#include "GUI_Looks.h"
#include "GuiXmlLoader.h"
//#include "vld.h"


SURFHANDLE IMS2::pilotPanelBG = NULL;
SURFHANDLE IMS2::engPanelBG = NULL;


DLLCLBK void InitModule (HINSTANCE hModule)
{
	//loading panel backgrounds and textures
/*	IMS2::pilotPanelBG = oapiLoadTexture ("IMS2\\IMSMainPanel.dds");
	IMS2::engPanelBG = oapiLoadTexture ("IMS2\\IMSEngPanel.dds");*/
//	IMS2::panelTextures = oapiLoadTexture ("IMS\\IMSPanelTextures.dds");

#ifdef _DEBUG
	Helpers::SetLogLevel(L_DEBUG);
#else 
	Helpers::SetLogLevel(L_WARNING);
#endif
	GuiXmlLoader::LoadStyleSets("Config/IMS2/GUI/Skins");
}

// --------------------------------------------------------------
// Module cleanup
// --------------------------------------------------------------
DLLCLBK void ExitModule (HINSTANCE hModule)
{
	oapiDestroySurface(IMS2::pilotPanelBG);
	oapiDestroySurface(IMS2::engPanelBG);
	
}

// --------------------------------------------------------------
// Vessel initialisation
// --------------------------------------------------------------
DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
/*	#ifdef _DEBUG
    // NOTE: _CRTDBG_CHECK_ALWAYS_DF is too slow
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
                   _CRTDBG_CHECK_CRT_DF | 
                   _CRTDBG_LEAK_CHECK_DF |
				   _CRTDBG_CHECK_ALWAYS_DF); 
	#endif*/
	return new IMS2(hvessel, flightmodel);
}


// --------------------------------------------------------------
// Vessel cleanup
// --------------------------------------------------------------
DLLCLBK void ovcExit (VESSEL *vessel) 
{
	if (vessel) delete (IMS2*)vessel;
}


IMS2::IMS2(OBJHANDLE hVessel, int flightmodel) : VESSEL4 (hVessel, flightmodel) 
{

	//initialise event handling
	eventhandlertype = VESSEL_HANDLER;
	//the EventGenerator that propagates events to modules
	createEventGenerator(VESSEL_TO_MODULE_PIPE);
	//a bit counterintuitive maybe, but of course the vessel needs an event sink that connects to its own generator, otherwise it doesn't get its own events.
	EventSink *temp = createEventSink(VESSEL_TO_MODULE_PIPE);
	ConnectToMyEventGenerator(temp, VESSEL_TO_MODULE_PIPE);

	//the EventSink that receives events from modules
	createEventSink(MODULE_TO_VESSEL_PIPE);

	//the EventSink that receives events from managers
	createEventSink(MANAGER_TO_VESSEL_PIPE);

	//the EventGenerator that propagates events to the vessel GUI
	createEventGenerator(VESSEL_TO_GUI_PIPE);

	//set the default event pipe
	defaultpipe = VESSEL_TO_MODULE_PIPE;

	InitialiseGUI();
	isSetForRedock = false;

	//create the managers
	managers[COG_MANAGER] = new IMS_CoGmanager(this);
	managers[PROP_MANAGER] = new IMS_PropulsionManager(this);
	managers[RCS_MANAGER] = new IMS_RcsManager(this);
	managers[TDPOINT_MANAGER] = new IMS_TouchdownPointManager(this);

	//create autopilot modes
	autopilots[KILLROT] = new IMS_Autopilot_Killrot(this);

	//add the sim started event. It will be fired in the first PreStep, when the event queue is processed for the first time.
	addEvent(new SimulationStartedEvent, VESSEL_TO_MODULE_PIPE);
}

IMS2::~IMS2() 
{
	DestroyGUI();

	//delete all module instances on this vessel
	for (UINT i = 0; i < modules.size(); ++i)
	{
		delete modules[i];
	}

	//delete the docked vessels list
	for (UINT i = 0; i < dockedVesselsList.size(); ++i)
	{
		delete dockedVesselsList[i];
	}

	//delete all managers
	for (map<IMS_MANAGER, IMS_Manager_Base*>::iterator i = managers.begin(); i != managers.end(); ++i)
	{
		delete i->second;
	}
}


void IMS2::clbkPreStep(double simt, double simdt, double mjd) 
{
	updateVesselState();
	sendEvents();

	//run through PreStep computations of modules on this vessel
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->PreStep(simdt);
	}

	//execute prestep on autopilots
	for (auto i = autopilots.begin(); i != autopilots.end(); ++i)
	{
		i->second->PreStep(simdt);
	}

	//execute prestep on all managers
	for (map<IMS_MANAGER, IMS_Manager_Base*>::iterator i = managers.begin(); i != managers.end(); ++i)
	{
		i->second->PreStep(simdt);
	}

	updateGui();

	preStateUpdate();
}


// ==============================================================
// Post step processings
// ==============================================================
void IMS2::preStateUpdate()
{
	//process the event waiting queue
	processWaitingQueue();

	//let all modules process their waiting queues
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->PreStateUpdate();
	}

	//let all managers process their waiting queue
	for (map<IMS_MANAGER, IMS_Manager_Base*>::iterator i = managers.begin(); i != managers.end(); ++i)
	{
		i->second->PreStateUpdate();
	}

}



void IMS2::clbkVisualCreated (VISHANDLE vis, int refcount) 
{
	hVis = vis;

	//we could not rotate the meshes when adding them to the vessel during loading
	//as weird as that sounds, but the vessels visual doesn't exist yet then!
	for (UINT i = 1; i < modules.size(); ++i)
	{
		modules[i]->RotateMesh();
	}
	addEvent(new VesselVisualCreatedEvent());
}

void IMS2::clbkVisualDestroyed (VISHANDLE vis, int refcount) 
{
	addEvent(new VesselVisualDestroyedEvent());
}



UINT IMS2::AddNewMesh(std::string meshName, VECTOR3 pos)
//either loads a new meshtemplate or gets an existing one, ats a new mesh instance to the visual and returns the index
{
	return AddMesh(oapiLoadMeshGlobal(meshName.data()), &pos);	
}


//ONLY CALL FROM OTHER IMS2 VESSEL ON ASSIMILATION
//passes pointers of own modules to received Vector
//sets own module pointers to NULL to prevent deallocation on vessel deletion
//receiving vessel will add module pointers to own module list
//VESSEL IS NO LONGER FUNCTIONAL AFTER EXECUTING THIS FUNCTION AND EXPECTS DELETION!

void IMS2::AssimilateMe(std::vector<IMS_Module*> &modulesToAssimilate)
{
	for (UINT i = 0; i < modules.size(); ++i)
	{
		//tranform module rot, dir and pos into global global reference
		VECTOR3 moduleGlobalRot, moduleGlobalDir, moduleGlobalPos;

		modules[i]->GetRot(moduleGlobalRot);
		modules[i]->GetDir(moduleGlobalDir);
		modules[i]->GetPos(moduleGlobalPos);
		moduleGlobalPos = GetPositionRelativeToCoG(moduleGlobalPos);

		GlobalRot(moduleGlobalRot, moduleGlobalRot);
		GlobalRot(moduleGlobalDir, moduleGlobalDir);
		Local2Global(moduleGlobalPos, moduleGlobalPos);

		modules[i]->SetRot(moduleGlobalRot);
		modules[i]->SetDir(moduleGlobalDir);
		modules[i]->SetPos(moduleGlobalPos);
	
		modulesToAssimilate.push_back(modules[i]);
		//modules[i] = NULL;
	}

	//remove the modules from this vessel
	while (modules.size() > 0)
	{
		RemoveModule(modules[0]);
	}
	modules.clear();
}


//assimilates ALL vessels which's names are in the passed list

void IMS2::AssimilateIMSVessels(vector<string> &vesList)
{
	for (UINT i = 0; i < vesList.size(); ++i)
	{
		VESSEL *ves = oapiGetVesselInterface(oapiGetVesselByName((char*)(vesList[i].data())));
		if (ves->Version() >= 2)
		{
			IMS2 *imsves = (IMS2*)(ves);
			if (imsves != NULL)
			{
				AssimilateIMSVessel(imsves);
			}
		}
	}

	Undock(ALLDOCKS);

	for (UINT i = 0; i < vesList.size(); ++i)
	//undock and delete assimilated vessels
	{
		VESSEL *ves = oapiGetVesselInterface(oapiGetVesselByName((char*)(vesList[i].data())));
		ves->Undock(ALLDOCKS);
		oapiDeleteVessel(ves->GetHandle(), GetHandle());
	}
	//notifier to check docking ports for overlap on clbkDockEvent
	isSetForRedock = true;
//	addEvent(new ShiftCGEvent(1));
	addEvent(new VesselLayoutChangedEvent);
}


void IMS2::AssimilateIMSVessel(IMS2 *vessel)
//calls AssimilateMe on other vessel to get its modules
//calculates module pos dir rot relative to this vessel
//adds new meshes to this
//ONLY CALL FROM AssimilateVessels()!!
{
	//get modules from other vessel
	vector<IMS_Module*> newModules;
	vessel->AssimilateMe(newModules);

	//update new module positions and add meshes
	for (UINT i = 0; i < newModules.size(); ++i)
	{
		VECTOR3 newRot, newDir, newPos;
		MATRIX3 localMatrix; 
		GetRotationMatrix(localMatrix);

		newModules[i]->GetRot(newRot);
		newModules[i]->GetDir(newDir);
		newModules[i]->GetPos(newPos);

		newRot = tmul(localMatrix, newRot);
		newDir = tmul(localMatrix, newDir);
		Global2Local(newPos, newPos);
		//the local position will be relative to the center of gravity, not to the actual position relative to the core module
		newPos += GetCoGmanager()->GetCoG();


		Calc::RoundVector(newPos, 1000);
		Calc::RoundVector(newRot, 1000);
		Calc::RoundVector(newDir, 1000);

		normalise(newRot);
		normalise(newDir);

		newModules[i]->SetRot(newRot);
		newModules[i]->SetDir(newDir);
		newModules[i]->SetPos(newPos);
		newModules[i]->AddModuleToVessel(this, true);
		newModules[i]->CreatePhysicalAttachmentPoints();

		newModules[i]->TransformAttachmentPoints();
		newModules[i]->SetDockPortsToNull();
		modules.push_back(newModules[i]);
	}
}


/* Splits the vessel into two vessels, with the modules in modules_to_remove
 * in the new vessel. The calling function is responsible for providing a list of modules
 * that actually makes sense (i.e. modules connected together, and not leaving
 * any modules hanging in the air). The new vessel will spawn docked in the same manner the 
 * modules were located before the split
 */
void IMS2::Split(vector<IMS_Module*> modules_to_remove)
{
	//if we don't get a valid list, don't bother
	if (modules_to_remove.size() == 0) return;			

	IMS_Module *newcore;
	VECTOR3 velocity;

	//find the first module that connects the modules to be split to the rest of the vessel
	//walk through all modules to be split off
	for (UINT i = 0; i < modules_to_remove.size(); ++i)
	{
		//get attachmentpoints of current module
		vector<IMSATTACHMENTPOINT*> points;
		modules_to_remove[i]->GetAttachmentPoints(points);
		//walk through attachmentpoints
		for (UINT j = 0; j < points.size(); ++j)
		{
			//check if the attachment point connects to another one, and that the other one is not on a docked vessel
			if (points[j]->connectsTo != NULL && points[j]->dockedVessel == NULL)
			{
				//get the module the point connects to
				IMS_Module *connectedmodule = points[j]->connectsTo->isMemberOf;
				//check if the connected module is not a module that is split of
				if (find(modules_to_remove.begin(), modules_to_remove.end(), connectedmodule) == modules_to_remove.end())
				{
					//if it isn't, it means that the split part connects to the rest of the vessel at this module and at this point
					//use the module as the new core of the split vessel
					newcore = modules_to_remove[i];
					//use the attachment point orientation as relative velocity vector
					velocity = points[j]->connectsTo->dir;
					break;
				}
			}
		}
	}

	RemoveModule(newcore);
	IMS2::SplitVesselData.modules.push_back(newcore);
	

	//remove modules from vessel and set their orientation and position relative to the new core module
	for (UINT i = 0; i < modules_to_remove.size(); ++i)
	{
		//of course, we should not do those things with the actual core
		if (modules_to_remove[i] != newcore)
		{
			RemoveModule(modules_to_remove[i]);
			modules_to_remove[i]->SetOrientationRelativeTo(newcore);
			IMS2::SplitVesselData.modules.push_back(modules_to_remove[i]);
		}
	}

	//delete all dockports on this vessel and set the dockports to NULL
	//if we do not delete the dockports first, the ports freed up by the split won't have the same indices as when the vessel is loaded from file
	ClearDockDefinitions();
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->SetDockPortsToNull();
	}

	//required for recreating and redocking dockports in next frame. 
	isSetForRedock = true;
	addEvent(new IMS_DockEvent);
//	addEvent(new ShiftCGEvent(1));

	//generate a name for the new vessel
	string basename = GetName();
	string newname = basename + "_";
	int i = 1;
	newname += to_string(i);
	while (oapiGetVesselByName((char*) newname.data()) != NULL)
	{
		i++;
		newname = basename + "_";
		newname += to_string(i);
	}

	//create the new vessel
	newcore->SpawnNewVesselFromModule(newname, velocity);
	addEvent(new VesselLayoutChangedEvent);
}



//completely removes a module from the vessel, but does NOT delete it in memory!
void IMS2::RemoveModule(IMS_Module* module)
{
	module->RemoveModuleFromVessel();
	vector<IMS_Module*>::iterator mi = find(modules.begin(), modules.end(), module);
	modules.erase(mi);
}



//overloaded event handler
bool IMS2::ProcessEvent(Event_Base *e)
{
	//relay all events to the GUI first
	relayEvent(e, VESSEL_TO_GUI_PIPE);


	if (*e == DOCKEVENT)
	{
		HandleDockEvent();
	}

	return false;
}




void IMS2::updateVesselState(bool triggerevents)
{
	//currently only checks whether the vessel has landed or lifted of.
	//add further state checks to this function when necessary
	DWORD fltstatus = GetFlightStatus();
	DWORD bitmask = 1;
	bool islandednow = (bool)(bitmask & fltstatus);

	//compare the vessel state to the state we have stored
	if (islandednow != islanded)
	{
		islanded = islandednow;
		//the state has changed, i.e. the vessel has either lifted off or touched down
		if (triggerevents)
		{
			if (islanded)
			{
				//the vessel has touched down
				addEvent(new TouchdownEvent());
			}
			else
			{
				//the vessel has lifted off
				addEvent(new LiftoffEvent);
			}
		}
	}
}


int IMS2::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (!KEYMOD_ALT(kstate) && !KEYMOD_SHIFT(kstate) && !KEYMOD_CONTROL(kstate) && key == OAPI_KEY_G && !islanded) {
		addEvent(new ToggleGearEvent(), VESSEL_TO_MODULE_PIPE);
		return 1;
	}
	return 0;
}


int IMS2::clbkNavProcess(int mode)
{
	if (GetRcsManager()->GetIntelligentRcs())
	{
		if (mode & NAVBIT_KILLROT != 0)
		{
			mode &= ~(1 << 0);
		}
	}

	return mode;
}

void IMS2::clbkNavMode(int  mode, bool  active)
{
	switch (mode)
	{
	case NAVMODE_KILLROT:
		autopilots[KILLROT]->SetActive(active);
		break;
	}
}
