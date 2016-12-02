#pragma once
#include "GUI_Common.h"
#include "GUI_elements.h"
#include "GUIentity.h"
#include "GUIplugin.h"
#include "GUImanager.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "Events.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_Module.h"
#include "Rotations.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunction_Factory.h"
#include "GUI_Module.h"
#include "IMS.h"
#include "Calc.h"
#include "IMS_Manager_Base.h"
#include "IMS_TouchdownPointManager.h"

IMS_Module::IMS_Module()
{
	_orbiter_data = NULL;
	_module_data = NULL;
	pos = _V(0,0,0);
	dir = _V(0,0,1);
	rot = _V(0,1,0);
	rotationmatrix = Rotations::UnitMatrix();

	//initialise event handling
	eventhandlertype = MODULE_HANDLER;
	//generator that propagates events to the vessel
	createEventGenerator(MODULE_TO_VESSEL_PIPE);
	//event sink that gets the own events directly from the generator
	EventSink *temp = createEventSink(MODULE_TO_VESSEL_PIPE);
	ConnectToMyEventGenerator(temp, MODULE_TO_VESSEL_PIPE);
	//sink that receives events from the vessel
	createEventSink(VESSEL_TO_MODULE_PIPE);
	//sink that receives events from module functions
	createEventSink(INSIDE_MODULE_PIPE);

}

IMS_Module::~IMS_Module()
{
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		delete attachmentPoints[i];
	}

	for (UINT i = 0; i < functions.size(); ++i)
	{
		delete functions[i];
	}
}


//shared functions/////////////////////////////////////////////////////

//called from IMS2::clbkSaveState() to write module state to scenario file
void IMS_Module::SaveState(FILEHANDLE scn, UINT index)
{
	stringstream ss("");
	ss << "MODULE_BEGIN_" << index;
	oapiWriteLine(scn, (char*)(ss.str().data()));
	SaveBaseAttributes(scn);
	oapiWriteLine(scn, "MODULE_END");
}

void IMS_Module::SaveBaseAttributes(FILEHANDLE scn)
{
	char buf[512];
	stringstream ss("");

	sprintf(buf, _orbiter_data->getConfigFileName().data());
	oapiWriteScenario_string(scn, "FILE", buf);

	ss << pos.x << "," << pos.y << "," << pos.z << " "
       << dir.x << "," << dir.y << "," << dir.z << " "
       << rot.x << "," << rot.y << "," << rot.z;
	sprintf(buf, ss.str().data());
	ss.str("");
	oapiWriteScenario_string(scn, "PDR", buf);

	   
	//saving IMS_ATTACHMENTS
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		ss << attachmentPoints[i]->exists << " " << attachmentPoints[i]->rotModifier;
		sprintf(buf, ss.str().data());
		ss.str("");
		oapiWriteScenario_string(scn, "IMSATT", buf);
	}

	//save ModuleFunctions
	for (UINT i = 0; i < functions.size(); ++i)
	{
		stringstream header;
		header << "BEGIN_MODULEFUNC " << i;
		oapiWriteLine(scn, (char*)header.str().data());
		functions[i]->SaveState(scn);
		oapiWriteLine(scn, "END_MODULEFUNC");
	}

	oapiWriteLine(scn, "END_BASE_ATTRIB");
}



bool IMS_Module::LoadState(FILEHANDLE scn)
{
	if (LoadScenarioBaseAttributes(scn))
	{
		return true;
	}
	return false;
}

bool IMS_Module::LoadScenarioBaseAttributes(FILEHANDLE scn)
{
	char *line;
	UINT attachmentPointsRead = 0;
	while (oapiReadScenario_nextline(scn, line) && _strnicmp(line, "END_BASE_ATTRIB", 16))
	{
		string s = line;

		if (s.compare(0, 3, "PDR") == 0)
		//reading pos dir rot from scenario
		{
			sscanf(s.substr(4).data(), "%lf,%lf,%lf %lf,%lf,%lf %lf,%lf,%lf",
				&(pos.x), &(pos.y), &(pos.z), 
				&(dir.x), &(dir.y), &(dir.z),
				&(rot.x), &(rot.y), &(rot.z));
		}
		else if (s.compare(0, 6, "IMSATT") == 0)
		{
			int mexists;
			double rotMod;
			sscanf(s.substr(7).data(), "%d %lf",
					&(mexists), &(rotMod));
			attachmentPoints[attachmentPointsRead]->exists = mexists != 0;
			RotateAttachmentPoint(attachmentPoints[attachmentPointsRead], rotMod);
			attachmentPointsRead++;
		}
		else if (s.compare(0, 16, "BEGIN_MODULEFUNC") == 0)
		{
			//load a module funcions state
			int idx;
				sscanf(s.substr(17).data(), "%d", &(idx));
				functions[idx]->LoadState(scn);
		}
	}
	//update the modules rotation matrices to the new orientation

	rotationmatrix = Rotations::GetRotationMatrixFromOrientation(dir, rot);

	TransformAttachmentPoints();
	return true;
}


// spawns a new vessel that has the same position, orientation and class as this module
// name: the name for the newly created vessel
// velocity: direction and magnitude of velocity relative to separating vessel

void IMS_Module::SpawnNewVesselFromModule(string _name, VECTOR3 velocity)
{
	//create the vessel status and fill it with the status of the current vessel
	VESSELSTATUS2 vs;
	memset(&vs, 0, sizeof(vs));
	vs.version = 2;
	vessel->GetStatusEx(&vs);

	//set relative position
	VECTOR3 relpos;
	vessel->Local2Rel(vessel->GetPositionRelativeToCoG(pos), relpos);
	vs.rpos = relpos;
	//set relative velocity
	VECTOR3 relvel;
	vessel->GlobalRot(velocity, relvel);
	vs.rvel += relvel;

	//build a global rotation matrix for the module by multiplying
	//the vessels global matrix with the modules local matrices
	MATRIX3 vesselglobalor_mat;
	MATRIX3 moduleglobalor_mat;

	vessel->GetRotationMatrix(vesselglobalor_mat);
	moduleglobalor_mat = mul(vesselglobalor_mat, rotationmatrix);

	//pass the rotation matrix into the SplitData so the newly created vessel gets them at creation
	IMS2::SplitVesselData.transform = moduleglobalor_mat;
	IMS2::SplitVesselData.split = true;

	//remove this module from the vessel
	//RemoveModuleFromVessel();

	//set the modules position and orientation to origin
	pos = _V(0, 0, 0);
	dir = _V(0, 0, 1);
	rot = _V(0, 1, 0);
	
	//create the new vessel
	oapiCreateVesselEx(_name.data(), _orbiter_data->getClassName().data(), &vs);
}


void IMS_Module::AddModuleToVessel(IMS2 *_vessel, bool rotatemesh, bool addMesh)
{
	//set the new core vessel and calculate the new rotation matrix for module-vessel transformations
	vessel = _vessel;
	rotationmatrix = Rotations::GetRotationMatrixFromOrientation(dir, rot);
	
	//add the mesh to the vessel, if it needs to be added 
	//(there are special circumstances where this is not necessary because it was already done by orbiter).
	if (addMesh)
	{
		AddMeshToVessel(rotatemesh);
	}
	else
	{
		//if the mesh doesn't need to be added, it means it's the first mesh on the vessel
		meshIndex = 0;
	}

	//If the module has a hullshape, add it to the vessel
	SimpleShape *hullshape = _orbiter_data->GetHullShape();
	if (hullshape != NULL)
	{
		vessel->GetTdPointManager()->AddHullShape(hullshape, pos, rotationmatrix);
	}

	//connect to the vessel to module event pipe
	vessel->ConnectToMyEventSink(getEventSink(VESSEL_TO_MODULE_PIPE), VESSEL_TO_MODULE_PIPE);
	//connect the vessel sink of the module to vessel pipe
	vessel->ConnectMySinkToYourNode(getEventSink(MODULE_TO_VESSEL_PIPE), MODULE_TO_VESSEL_PIPE);

	if (functions.size() > 0)
	{
		//tell all the module functions to add themselves to the vessel
		for (UINT i = 0; i < functions.size(); ++i)
		{
			functions[i]->AddFunctionToVessel(vessel);
		}
	}

	//if the module has gui functionality in its functions, plug it into the modules control page
	if (guiplugin != NULL)
	{
		vessel->GetGUI()->GetElementById(MD_MODULES_CTRLPG)->AddPlugin(guiplugin);
	}
}



bool IMS_Module::AddMeshToVessel(bool rotateMesh)
{

	meshIndex = vessel->AddNewMesh(_orbiter_data->getMeshName(), vessel->GetPositionRelativeToCoG(pos));

	if (rotateMesh)
	{
		RotateMesh();
	}

	vessel->SetMeshVisibilityMode(meshIndex, MESHVIS_EXTERNAL | MESHVIS_VC);
	return true;
}


//removes the module mesh from the vessel. Usually this is only called by RemoveModuleFromVessel
void IMS_Module::RemoveMeshFromVessel()
{
	vessel->DelMesh(meshIndex);
}


/* removes the module from the vessel and unregisteres it at the appropriate places
 * overload for module types that register things or need to modify the base vessel
 * overloaded methods should ALWAYS invoke the base method because of attachment point
 * and mesh handling!
 * does NOT remove the module pointer from the vessels modules list!
 */
 void IMS_Module::RemoveModuleFromVessel()
{

	//unplug the module gui if it has one
	if (guiplugin != NULL)
	{
		vessel->GetGUI()->GetElementById(MD_MODULES_CTRLPG)->RemovePlugin(guiplugin);
	}

	//tell the module functions to remove themselves
	for (UINT i = 0; i < functions.size(); ++i)
	{
		functions[i]->RemoveFunctionFromVessel(vessel);
	}

	//remove mesh and attachment points
	RemoveMeshFromVessel();
	RemoveAllAttachmentPoints();
	
	//remove hullshape, if the module has one
	SimpleShape *hullshape = _orbiter_data->GetHullShape();
	if (hullshape != NULL)
	{
		vessel->GetTdPointManager()->RemoveHullShape(hullshape);
	}

	//disconnect events
	disconnect(vessel);
}

 

/* this function places the module so its relative position and orientation to module is conserved if
 * module rotates to 0, 0, 1 / 0, 1, 0 (origin). This is used to preserve the relative orientation
 * of modules in a subset of the vessel that is being split off
 */
void IMS_Module::SetOrientationRelativeTo(IMS_Module *module)
{
	//if the reference module is aligned with the vessel, the other modules relative orientations don't have to be changed either
	if (Rotations::RequiresRotation(module->dir, module->rot))
	{
		//get the inverted matrix of module's initial orientation
		MATRIX3 reldirmat = Rotations::InverseMatrix(module->rotationmatrix);

		//rotate dir and rot back by as much as module was rotated back
		dir = mul(reldirmat, dir);
		rot = mul(reldirmat, rot);

		//do the same for position while translating it to keep relative position with module
		pos = mul(reldirmat, pos - module->pos);
	}
	//we still need to update the position, though
	else
	{
		pos = pos - module->pos;
	}
	//round the stuff, because doubles...
	Calc::RoundVector(pos, 1000);
	Calc::RoundVector(dir, 1000);
	Calc::RoundVector(rot, 1000);
}


void IMS_Module::RotateMesh()
{
	if (Rotations::RequiresRotation(dir, rot))
	{
		MESHHANDLE meshTemplate = vessel->GetMeshTemplate(meshIndex);
		DEVMESHHANDLE devMesh = vessel->GetDevMesh(vessel->GetVisHandle(), meshIndex);

		Rotations::TransformMesh(meshTemplate, devMesh, rotationmatrix);
	}
}


/*void IMS_Module::SetNewVessel(IMS2 *newVessel)
{
	vessel = newVessel;
}*/

void IMS_Module::GetRot(VECTOR3 &mRot)
{
	mRot = rot;
}

void IMS_Module::GetDir(VECTOR3 &mDir)
{
	mDir = dir;
}

void IMS_Module::GetPos(VECTOR3 &mPos)
{
	mPos = pos;
}


void IMS_Module::SetRot(VECTOR3 &mRot)
{
	rot = mRot;
}

void IMS_Module::SetDir(VECTOR3 &mDir)
{
	dir = mDir;
}

void IMS_Module::SetPos(VECTOR3 &mPos)
{
	pos = mPos;
}


//creates a new module from data already loaded from config file
bool IMS_Module::InitModule(IMS_Orbiter_ModuleData *orbiter_data, IMS_General_ModuleData *module_data, IMS2 *mVessel)
{
	vessel = mVessel;
	_orbiter_data = orbiter_data;
	_module_data = module_data;

	CreatePhysicalAttachmentPoints();

	//create the module functionalities
	vector<IMS_ModuleFunctionData_Base*> functiondata = module_data->GetFunctionData();
	int guiypos = 10;
	for (UINT i = 0; i < functiondata.size(); ++i)
	{
		//create a GUIplugin for the module if a) it hasn't been created yet and b) any of the functions actually have a gui
		if (guiplugin == NULL && functiondata[i]->HasGui())
		{
			//the modules gui plugin needs to be created before the first modulefunction with a gui
			createGui();
		}
		functions.push_back(IMS_ModuleFunction_Factory::CreateNewModuleFunction(functiondata[i], this));

		if (functiondata[i]->HasGui())
		{
			//set the vertical position of the modulefunctions gui
			GUI_ModuleFunction_Base *curgui = functions[functions.size() - 1]->GetGui();
			curgui->SetYpos(guiypos);
			RECT guirect;
			curgui->GetRect(guirect);
			guiypos = guirect.bottom + 5;
		}
	}

	//after positioning the module function guis, tell the 
	//module GUI to calculate its scrollrange
	if (guiplugin != NULL)
	{
		GUI_ScrollablePage *modulegui = (GUI_ScrollablePage*)(guiplugin->GetElementById(GUI_MODULE_PAGE));
		modulegui->CalculateScrollRange();
	}



	return true;
}


void IMS_Module::createGui()
{
	//first check if the GUI hasn't already been created.
	if (guiplugin == NULL)
	{
		//create a gui plugin to display module function controls
		guiplugin = new GUIplugin();
		//create the modules own GUI page and register it in the plugin
		GUI_Module *gui = new GUI_Module(vessel->GetGUI(), this);
	}
}

//function executed at the end of clbkLoadState
//things that need to be done after initialising everything, but before clbkPostCreation
void IMS_Module::PostLoad()
{
	for (UINT i = 0; i < functions.size(); ++i)
	{
		functions[i]->PostLoad();
	}
	
}


bool IMS_Module::ProcessEvent(Event_Base *e)
{

	//mostly what we do here is forward events from the module funtions to the vessel,
	//if they are events that are meant for the vessel. 
	if (e->GetEventPipe() == INSIDE_MODULE_PIPE)
	{
		if (*e == MASSHASCHANGEDEVENT)
		{
			relayEvent(e, MODULE_TO_VESSEL_PIPE);
			return true;
		}
		else if (*e == CONSUMABLEADDEDEVENT)
		{
			relayEvent(e, MODULE_TO_VESSEL_PIPE);
		}
	}
	return false;
}




void IMS_Module::PreStep(double simdt)
{
	sendEvents();

	//run through PreStep computations of ModuleFunctions in this module
	for (UINT i = 0; i < functions.size(); ++i)
	{
		functions[i]->PreStep(simdt, vessel);
	}
}


void IMS_Module::PreStateUpdate()
{
	//process the waiting queue on the module.
	processWaitingQueue();

	//let all modulefunctions process theirs.
	for (UINT i = 0; i < functions.size(); ++i)
	{
		functions[i]->PreStateUpdate();
	}
}


double IMS_Module::GetMass()
{
	double totalmass = _module_data->getMass();
	for (UINT i = 0; i < functions.size(); ++i)
	{
		totalmass += functions[i]->GetMass();
	}
	return totalmass;
}


double IMS_Module::GetSize()
{
	return _orbiter_data->getSize();
}

void IMS_Module::GetPmi(VECTOR3 &pmi)
{
	_orbiter_data->getPmi(pmi);
}

VECTOR3 IMS_Module::GetLocalCoordinates()
{ 
	return vessel->GetPositionRelativeToCoG(pos); 
}


//returns name and position of a module in a string for use in a listbox
string IMS_Module::getModulePosDescription()
{
	stringstream moduledescription;
	moduledescription << _module_data->getName() << " (" <<
		pos.x << ", " << pos.y << ", " << pos.z << ")";
	return moduledescription.str();
}


