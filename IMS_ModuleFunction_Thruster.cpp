#include "Common.h"
#include "Managers.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "IMS_PropulsionManager.h"
#include "GuiIncludes.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Thruster.h"
#include "GUI_ModuleFunction_Thruster.h"


IMS_ModuleFunction_Thruster::IMS_ModuleFunction_Thruster(IMS_ModuleFunctionData_Thruster *_data, IMS_Module *_module, bool creategui)
	: IMS_ModuleFunction_Base(_data, _module, MTYPE_THRUSTER), data(_data)
{
	if (creategui)
	{
		//create the thruster's menu
		menu = new GUI_ModuleFunction_Thruster(this, module->GetGui());
	}


}


IMS_ModuleFunction_Thruster::~IMS_ModuleFunction_Thruster()
{
}

void IMS_ModuleFunction_Thruster::PostLoad()
{
	//set the initially selected group in the gui. No group is default
	if (thrustergroup != -1)
	{
		menu->SetThGroupRadioBtn(thrustergroup);
	}
	
	//set the initially selected mode in the gui. Mode 0 is default.
	if (currentthrustermode != 0)
	{
		menu->SetThModeSelection(currentthrustermode);
	}
}

void IMS_ModuleFunction_Thruster::AddFunctionToVessel(IMS2 *vessel)
{
	propmanager = vessel->GetPropulsionManager();
	
	//tranform the thrust direction to vessel relative coordinates
	MATRIX3 modulematrix = module->GetRotationMatrix();
	VECTOR3 thrustdirection = mul(modulematrix, data->GetThrustDirection());
	normalise(thrustdirection);
	VECTOR3 thrusterposition = mul(modulematrix, data->GetThrusterPosition());
	thrusterposition += module->GetLocalCoordinates();
	//create the thruster on the vessel and store the handle
	thruster = propmanager->AddThruster(data->GetThrusterMode(0), thrusterposition, thrustdirection);
	
	//add the thruster to a thrustergroup if it has one assigned
	if (thrustergroup != -1)
	{
		propmanager->AddThrusterToGroup(thruster, (THGROUP_TYPE)thrustergroup);
	}

	AddExhaustsToVessel();

}


void IMS_ModuleFunction_Thruster::RemoveFunctionFromVessel(IMS2 *vessel)
{
	//remove the thrusters exhausts
	propmanager->RemoveExhausts(thruster);
	//calculate the flowrate of the thruster so the propellant manager can readjust the size of the virtual resource
	THRUSTERMODE *curmode = data->GetThrusterMode(currentthrustermode);
	double maxmassflow = curmode->Thrust / curmode->Isp;
	//remove the thruster from any groups if it is assigned to one
	if (thrustergroup != -1)
	{
		propmanager->RemoveThrusterFromGroup(thruster, (THGROUP_TYPE)thrustergroup);
	}
	//remove the thruster completely from the vessel
	propmanager->RemoveThruster(thruster, maxmassflow);
}


void IMS_ModuleFunction_Thruster::AddExhaustsToVessel()
{
	//calculate vessel-relative exhausts and add them to the vessel
	vector<THRUSTEREXHAUST> relexhausts = getRelativeExhausts();
	propmanager->AddExhausts(thruster, relexhausts);
}

void IMS_ModuleFunction_Thruster::RemoveExhaustsFromVessel()
{
	propmanager->RemoveExhausts(thruster);
}


void IMS_ModuleFunction_Thruster::PreStep(double simdt, IMS2 *vessel)
{

}


bool IMS_ModuleFunction_Thruster::ProcessEvent(Event_Base *e)
{
	if (*e == CGHASCHANGEDEVENT)
	{
		//the cg has been changed, we'll need to move the exhausts, since orbiter happily ignores them!
		RemoveExhaustsFromVessel();
		AddExhaustsToVessel();
	}
	return false;
}


GUI_ModuleFunction_Base *IMS_ModuleFunction_Thruster::GetGui()
{
	return menu;
}

void IMS_ModuleFunction_Thruster::SetThrusterMode(int modeidx)
{
	Helpers::assertThat([modeidx, this]() { return modeidx < data->getNumberOfModes(); }, "Thrustermode index out of bounds!");
	
	//only switch if something the mode is not the same as it is currently
	if (modeidx != currentthrustermode)
	{
		currentthrustermode = modeidx;
		propmanager->SwitchThrusterMode(thruster, data->GetThrusterMode(modeidx));
		//redefine the exhausts
		RemoveExhaustsFromVessel();
		AddExhaustsToVessel();
	}
}


void IMS_ModuleFunction_Thruster::SetThrusterGroup(int thrustergroup)
{
	//input validation, since not all thrustergroups are valid for normal thrusters
	Helpers::assertThat([thrustergroup]() { return (thrustergroup >= -1 && thrustergroup <= (int)THGROUP_HOVER) ||
		thrustergroup == THGROUP_USER; }, "Not assigning to a valid thrustergroup!");
	
	//check if the thrustergroup changes
	if (thrustergroup != this->thrustergroup)
	{
		//check if the thruster is currently assigned to any group, and if yes, remove it
		if (this->thrustergroup != -1)
		{
			propmanager->RemoveThrusterFromGroup(thruster, (THGROUP_TYPE)this->thrustergroup);
		}
		//check if the thruster gets assigned to a new thrustergroup, and add if yes
		if (thrustergroup != -1)
		{
			propmanager->AddThrusterToGroup(thruster, (THGROUP_TYPE)thrustergroup);
		}
		this->thrustergroup = thrustergroup;
	}
}


void IMS_ModuleFunction_Thruster::SaveState(FILEHANDLE scn)
{
	//write the current thrustermode to the scenario
	if (currentthrustermode != 0)
	{
		oapiWriteScenario_int(scn, "THMODE", currentthrustermode);
	}
	//write the current thruster group to the scenario
	if (thrustergroup != -1)
	{
		oapiWriteScenario_int(scn, "THGROUP", thrustergroup);
	}
}

bool IMS_ModuleFunction_Thruster::processScenarioLine(string line)
{
	//see if a mode or a group was saved
	if (line.substr(0, 6) == "THMODE")
	{
		currentthrustermode = atoi(line.substr(7).data());
		return true;
	}
	else if (line.substr(0, 7) == "THGROUP")
	{
		thrustergroup = atoi(line.substr(8).data());
		return true;
	}
	return false;
}


vector<THRUSTEREXHAUST> IMS_ModuleFunction_Thruster::getRelativeExhausts()
{
	int n_exhausts = data->GetNumberOfExhausts();
	vector<THRUSTEREXHAUST> newexhausts;
	THRUSTERMODE *curmode = data->GetThrusterMode(currentthrustermode);

	for (int i = 0; i < n_exhausts; ++i)
	{
		THRUSTEREXHAUST *curexhaust = data->GetThrusterExhaust(i);

		//rotate and translate the exhaust position to vessel-relative coordinates
		MATRIX3 modulerot = module->GetRotationMatrix();
		VECTOR3 exhaustpos = mul(modulerot, curexhaust->pos);
		exhaustpos += module->GetLocalCoordinates();
		//rotate the exhaust direction to vessel-relative direction
		VECTOR3 exhaustdir = mul(modulerot, curexhaust->dir);
		normalise(exhaustdir);

		//create a new exhaust definition with the vessel-relative values
		THRUSTEREXHAUST newexhaust;
		newexhaust.dir = exhaustdir;
		newexhaust.pos = exhaustpos;
		newexhaust.length = curexhaust->length * curmode->Exhaustlength;
		newexhaust.width = curexhaust->width * curmode->Exhaustwidth;

		//add the new exhaust definition to the list of exhausts to be created
		newexhausts.push_back(newexhaust);
	}
	return newexhausts;
}