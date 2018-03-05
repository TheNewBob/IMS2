#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "GuiIncludes.h"
#include "Managers.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Location.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_Movable.h"
#include "IMS_Storable.h"
#include "IMS_Manager_Base.h"
#include "IMS_PropulsionManager.h"
#include "IMS_ModuleFunctionData_Tank.h"
#include "GUI_ModuleFunction_Base.h"
#include "GUI_ModuleFunction_Tank.h"
#include "IMS_ModuleFunction_Tank.h"



IMS_ModuleFunction_Tank::IMS_ModuleFunction_Tank(IMS_ModuleFunctionData_Tank *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Pressurised(_data, _module, MTYPE_TANK, { IMS_Location::CONTEXT_NONTRAVERSABLE }), data(_data)
{
	menu = new GUI_ModuleFunction_Tank(this, module->GetGui());
}


IMS_ModuleFunction_Tank::~IMS_ModuleFunction_Tank()
{
}

void IMS_ModuleFunction_Tank::PostLoad()
{
	if (tank == NULL)
	{
		//if the state wasn't loaded, create the tank at full capacity
		tank = new IMS_Storable(data->getVolume(), data->getPropellant(), this);
	}
	menu->Init();
}



void IMS_ModuleFunction_Tank::AddFunctionToVessel(IMS2 *vessel)
{
	propmanager = vessel->GetPropulsionManager();
	propmanager->AddTank(tank);
}

void IMS_ModuleFunction_Tank::RemoveFunctionFromVessel(IMS2 *vessel)
{
	propmanager->RemoveTank(tank);
	propmanager = NULL;
};


void IMS_ModuleFunction_Tank::SetPropellantType(int propellant_id)
{
	//check if the tank is empty. Only empty tanks can be reassigned to another propellant type!
	if (tank != NULL && propellant_id != tank->GetConsumableId())
	{
		//tank has already been initialised and the type changes. Destroy the old tank, but only if it's empty
		if (tank->GetMass() == 0.0)
		{
			propmanager->RemoveTank(tank);
			this->RemoveMovable(tank);
			delete tank;
			tank = NULL;
		}
		else
		{
			return;
		}
	}

	if (tank == NULL)
	{
		//create a new tank with the new propellant type
		CONSUMABLEDATA *newprop = IMS_ModuleDataManager::GetConsumableData(propellant_id);
		tank = new IMS_Storable(data->getVolume(), newprop, this, 0.0);
		propmanager->AddTank(tank);
		//update the menu with the new information
		menu->Init();
	}

}


bool IMS_ModuleFunction_Tank::ProcessEvent(Event_Base *e)
{
	return false;
}

void IMS_ModuleFunction_Tank::SaveState(FILEHANDLE scn)
{
	//save the current consumable type if it is 
	oapiWriteScenario_string(scn, "STORABLE", (char*)tank->Serialize().data());
}


bool IMS_ModuleFunction_Tank::processScenarioLine(string line)
{
	if (line.substr(0, 8) == "STORABLE")
	{
		//load the storable from its serialisation data
		tank = new IMS_Storable(line.substr(9), this);
		return true;
	}

	return false;
}

GUI_ModuleFunction_Base *IMS_ModuleFunction_Tank::GetGui()
{
	return menu;
}

void IMS_ModuleFunction_Tank::SetValveStatus(bool open)
{
	if (tank->IsAvailable() != open)
	{
		if (open)
		{
			//the valve is closed an must be opened
			propmanager->OpenTank(tank);
		}
		else
		{
			//the valve is open and must be closed
			propmanager->CloseTank(tank);
		}
		
		//update the gui. It was probably the one that called this function, but that is not guaranteed.
		menu->SetValveBox(open);
	}
}

bool IMS_ModuleFunction_Tank::GetValveStatus()
{
	return tank->IsAvailable();
}




//callback function for setting the propellant type
bool PropellantTypeSelected(int idx, void *tank)
{
	//if the index is -1, cancel was clicked
	if (idx != -1)
	{
		IMS_ModuleFunction_Tank *_tank = (IMS_ModuleFunction_Tank*)tank;

		//the index passed is not the consumable id. It's the selected item in the list.
		//so to know what was actually chosen, we need to recreate the list
		//get a list with consumables that could be stored in this tank
		vector<CONSUMABLEDATA*> proplist = IMS_ModuleDataManager::GetCompatibleConsumables(_tank->GetTank()->GetConsumableId());
		_tank->SetPropellantType(proplist[idx]->id);
	}
	return true;
}