#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "Events.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Comm.h"
#include "IMS_ModuleFunctionData_Thruster.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Thruster.h"
#include "GUI_ModuleFunction_Thruster.h"


GUI_ModuleFunction_Thruster::GUI_ModuleFunction_Thruster(IMS_ModuleFunction_Thruster *thruster, GUIplugin *gui)
	: GUI_ModuleFunction_Base(getHeight(thruster->GetData()->getNumberOfModes()), gui, gui->GetStyle(STYLE_BUTTON)), basethruster(thruster)
{
	//Add the name label
	gui->CreateLabel(basethruster->GetData()->GetName(), _R(10, 10, width - 10, 35), id);

	//if there is more than one thrustermode, create a selection for them
	int nummodes = thruster->GetData()->getNumberOfModes();
	if (nummodes > 1)
	{
		//only create a select box if there is more than one mode
		gui->CreateLabel("Mode", _R(15, 40, 60, 65), id);
		//the select box will stretch a bit for more modes, but will not display more than 4 entries without scrolling
		modesbox = gui->CreateListBox(_R(15, 70, width - 10, min(nummodes * 25 + 70, 170)), id);
		for (UINT i = 0; i < (UINT)nummodes; ++i)
		{
			THRUSTERMODE *curmode = thruster->GetData()->GetThrusterMode(i);
			string description = curmode->Name + ";   Ve: " + Helpers::doubleToUnitString(curmode->Isp, "m/s")
				+ "; Thrust: " + Helpers::doubleToUnitString(curmode->Thrust, "N");
			modesbox->AddElement(description);
		}
		modesbox->SetSelected(0);
	}
	else
	{
		//otherwise, just print the attributes
		THRUSTERMODE *mode = thruster->GetData()->GetThrusterMode(0);
		gui->CreateLabelValuePair("Ve:", Helpers::doubleToUnitString(mode->Isp, "m/s"), _R(15, 70, 165, 95), id, -1, STYLE_DEFAULT);
		gui->CreateLabelValuePair("Thrust:", Helpers::doubleToUnitString(mode->Isp, "N"), _R(170, 70, 320, 95), id, -1, STYLE_DEFAULT);
	}

	//starting y position after the listbox
	int  cury = min(nummodes, 4) * 25 + 80;

	//create label and radio buttons for thruster group assignement
	gui->CreateLabel("Group", _R(15, cury, 70, cury  +  25), id);
	groupbuttons.push_back(gui->CreateRadioButton("1", _R(15, cury + 30, 65, cury + 55), id));
	groupbuttons.push_back(gui->CreateRadioButton("2", _R(70, cury + 30, 110, cury + 55), id));
	groupbuttons.push_back(gui->CreateRadioButton("3", _R(115, cury + 30, 165, cury + 55), id));
	groupbuttons.push_back(gui->CreateRadioButton("4", _R(170, cury + 30, 210, cury + 55), id));
	groupbuttons.push_back(gui->CreateRadioButton("none", _R(215, cury + 30, 300, cury + 55), id));
	GUI_RadioButton::CreateGroup(groupbuttons);
	//default checked is always 4th button, "no group"
	groupbuttons[4]->SetChecked();
}

GUI_ModuleFunction_Thruster::~GUI_ModuleFunction_Thruster()
{
}


int GUI_ModuleFunction_Thruster::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	if (eventId != -1)
	{
		if (modesbox != NULL && eventId == modesbox->GetId())
		{
			//thrustermode was changed
			basethruster->SetThrusterMode(modesbox->GetSelected());
		}
		else if (eventId == groupbuttons[0]->GetId())
		{
			//radio button was changed to group 1
			basethruster->SetThrusterGroup((int)THGROUP_MAIN);
		}
		else if (eventId == groupbuttons[1]->GetId())
		{
			//radio button was changed to group 2
			basethruster->SetThrusterGroup((int)THGROUP_RETRO);
		}
		else if (eventId == groupbuttons[2]->GetId())
		{
			//radio button was changed to group 3
			basethruster->SetThrusterGroup((int)THGROUP_HOVER);
		}
		else if (eventId == groupbuttons[3]->GetId())
		{
			//radio button was changed to group 4
			basethruster->SetThrusterGroup((int)THGROUP_USER);
		}
		else if (eventId == groupbuttons[4]->GetId())
		{
			//radio button was changed to no group
			basethruster->SetThrusterGroup(-1);
		}
	}
	return eventId;
}


void GUI_ModuleFunction_Thruster::SetThGroupRadioBtn(int setbtn)
{
	assert(setbtn >= 0 && setbtn <= 4 && "selected radio button must be >= 0, <= 4");
	groupbuttons[setbtn]->SetChecked();
}

void GUI_ModuleFunction_Thruster::SetThModeSelection(int setmode)
{
	assert(setmode >= 0 && setmode < modesbox->NumEntries() && "The set mode is hiogher than the number of modes available!");
	modesbox->SetSelected(setmode);
}

int GUI_ModuleFunction_Thruster::getHeight(int numthrustermodes)
{
	// We'll display up to four thrustermodes without scrolling, plus some space for the group selection plus some padding
	return min(numthrustermodes, 4) * 25 + 150;
}