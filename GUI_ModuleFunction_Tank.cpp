#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "Events.h"
#include "IMS_General_ModuleData.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_Movable.h"
#include "IMS_Storable.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_ModuleFunctionData_Tank.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_ModuleFunction_Tank.h"
#include "GUI_ModuleFunction_Tank.h"

GUI_ModuleFunction_Tank::GUI_ModuleFunction_Tank(IMS_ModuleFunction_Tank *tank, GUIplugin *gui)
	: GUI_ModuleFunction_Base(100, gui, gui->GetStyle(STYLE_DEFAULT)), tank(tank)
{
	proplabel = gui->CreateLabel("placeholder", _R(60, 10, width - 60, 35), id);
	status = gui->CreateStatusBar(_R(10, 40, width - 10, 65), id);
	valve = gui->CreateCheckBox("valve open", _R(10, 70, width / 2, 95), id);
	valve->SetChecked(true);
	proptypebtn = gui->CreateDynamicButton("designate propellant", _R(width - 160, 70, width - 10, 95), id);
	proptypebtn->SetVisible(false);

//create cheatbutton in debug builds. Made permanent for now.	
//#ifdef _DEBUG
	cheatbutton = gui->CreateDynamicButton("fill", _R(width - 60, 10, width - 10, 35), id);
//#endif
}

void GUI_ModuleFunction_Tank::Init()
{
	IMS_Storable *storable = tank->GetTank();
	proplabel->ChangeText(storable->GetConsumableName());
	//initialise the status bar that shows how full the tank is
	status->SetVerbose(true);
	status->SetCapacity(storable->GetCapacity(), "kg");
	
	//note the current propellant mass in the tank and set the status bar accordingly
	currentcontent = tank->GetMass();
	//show the button to set the propellant type if the tank is initialised empty
	if (currentcontent == 0.0)
	{
		proptypebtn->SetVisible(true);
	}

	//initialise the status bar
	status->SetFillStatusByAmount(currentcontent);

	//initialise checkbox to open and close the propellant valve
	valve->SetChecked(tank->GetValveStatus());
}

GUI_ModuleFunction_Tank::~GUI_ModuleFunction_Tank()
{
}

void GUI_ModuleFunction_Tank::SetValveBox(bool checked)
{
	if (checked != valve->Checked())
	{
		valve->SetChecked(checked);
		updatenextframe = true;
	}
}

bool GUI_ModuleFunction_Tank::updateMe()
{
	double propmass = tank->GetMass();
	bool mustredraw = false;
	if (updatenextframe)
	{
		mustredraw = true;
		updatenextframe = false;
	}
	
	// check every 0.1 secs if any propellant has been consumed	
	if (GUImanager::CheckSynchronisedInterval(100) &&
		currentcontent != propmass)
	{
		//the propellant in the tank has changed, update the status bar and redraw
		currentcontent = propmass;
		status->SetFillStatusByAmount(currentcontent);
		//show or hide the button to assign a new propellant type
		if (propmass == 0.0)
		{
			proptypebtn->SetVisible(true);
		}
		else
		{
			proptypebtn->SetVisible(false);
		}
		return true;
	}
	return mustredraw;
}


int GUI_ModuleFunction_Tank::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	if (eventId != -1)
	{
		if (eventId == valve->GetId())
		{
			//the valve has been opened or closed
			tank->SetValveStatus(valve->Checked());
		}
		else if (eventId == proptypebtn->GetId())
		{
			//get a list with consumables that could be stored in this tank
			vector<CONSUMABLEDATA*> proplist = IMS_ModuleDataManager::GetCompatibleConsumables(tank->GetTank()->GetConsumableId());
			//get their names into a list
			vector<string> propnames(proplist.size());
			for (UINT i = 0; i < proplist.size(); ++i)
			{
				propnames[i] = proplist[i]->name;
			}
			
			GUImanager::ListDialog("choose propellant", propnames, tank->GetModule()->GetGui()->GetFirstVisibleChild(), &PropellantTypeSelected, (void*)tank);
		}
		else if (cheatbutton != NULL && eventId == cheatbutton->GetId())
		{
			tank->GetTank()->Fill();
		}
	}
	
	return eventId;
}

