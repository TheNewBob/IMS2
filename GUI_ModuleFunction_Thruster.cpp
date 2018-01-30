#include "GUI_Common.h"
#include "Common.h"
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
#include "LayoutManager.h"

const static string FILENAME = "modulefunctions/thruster.xml";
const static string THRUSTER_NAME_LABEL = "thruster_name_label";
const static string GROUPS_LABEL = "groups_label";
const static string GROUP_1_CHKBX = "group_1_chkbx";
const static string GROUP_2_CHKBX = "group_2_chkbx";
const static string GROUP_3_CHKBX = "group_3_chkbx";
const static string GROUP_4_CHKBX = "group_4_chkbx";
const static string GROUP_NONE_CHKBX = "group_none_chkbx";
const static string MODES_LABEL = "modes_label";
const static string MODES_LIST = "modes_list";


GUI_ModuleFunction_Thruster::GUI_ModuleFunction_Thruster(IMS_ModuleFunction_Thruster *thruster, GUIplugin *gui)
	: GUI_ModuleFunction_Base(getHeight(thruster->GetData()->getNumberOfModes()), gui, gui->GetStyle(IMS_ModuleFunction_Base::GUI_MODULE_STYLE)), 
	basethruster(thruster)
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(FILENAME);

	//Add the name label
	gui->CreateLabel(getLayoutDataForElement(THRUSTER_NAME_LABEL, l), basethruster->GetData()->GetName(), id);

	//create label and radio buttons for thruster group assignement
	gui->CreateLabel(getLayoutDataForElement(GROUPS_LABEL, l), "Group", id);
	groupbuttons.push_back(gui->CreateRadioButton(getLayoutDataForElement(GROUP_1_CHKBX, l), "1", id));
	groupbuttons.push_back(gui->CreateRadioButton(getLayoutDataForElement(GROUP_2_CHKBX, l), "2", id));
	groupbuttons.push_back(gui->CreateRadioButton(getLayoutDataForElement(GROUP_3_CHKBX, l), "3", id));
	groupbuttons.push_back(gui->CreateRadioButton(getLayoutDataForElement(GROUP_4_CHKBX, l), "4", id));
	groupbuttons.push_back(gui->CreateRadioButton(getLayoutDataForElement(GROUP_NONE_CHKBX, l), "none", id));
	GUI_RadioButton::CreateGroup(groupbuttons);
	//default checked is always 4th button, "no group"
	groupbuttons[4]->SetChecked();

	// the field for the thrustermode will need a bit of dynamic adjustment. We'll take the layout size as maximum.
	LAYOUTDATA modesdata = getLayoutDataForElement(MODES_LIST, l);

	//if there is more than one thrustermode, create a selection for them
	int nummodes = thruster->GetData()->getNumberOfModes();
	if (nummodes > 1)
	{
		//only create a select box if there is more than one mode
		gui->CreateLabel(getLayoutDataForElement(MODES_LABEL, l), "Mode", id);
		//the layout size is the maximum size of the list, but if there's fewer modes, make it smaller.
		int fontheight = style->GetFont()->GetfHeight();
		modesdata.rect.bottom = min(modesdata.rect.bottom, modesdata.rect.top + (nummodes + 1) * fontheight);	//one line is for top and bottom padding
		modesbox = gui->CreateListBox(modesdata, id);
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
		//if there's only one mode, just print the thrusters properties.
		THRUSTERMODE *mode = thruster->GetData()->GetThrusterMode(0);
		gui->CreateLabel(getLayoutDataForElement(MODES_LABEL, l), "Properties", id);
		int fontheight = style->GetFont()->GetfHeight();
		modesdata.rect.bottom = modesdata.rect.top + fontheight;
		//split the width of the modesrect into two.
		int originalright = modesdata.rect.right;
		modesdata.rect.right = modesdata.rect.left + modesdata.rect.right / 2;
		gui->CreateLabelValuePair(modesdata, "Ve:", Helpers::doubleToUnitString(mode->Isp, "m/s"), id, -1);
		modesdata.rect.right = originalright;
		modesdata.rect.left = modesdata.rect.left + modesdata.rect.right / 2;
		gui->CreateLabelValuePair(modesdata, "Thrust:", Helpers::doubleToUnitString(mode->Isp, "N"), id, -1);
	}

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
	Helpers::assertThat([setbtn]() { return setbtn >= 0 && setbtn <= 4; }, "selected radio button must be >= 0, <= 4");
	groupbuttons[setbtn]->SetChecked();
}

void GUI_ModuleFunction_Thruster::SetThModeSelection(int setmode)
{
	Helpers::assertThat([setmode]() { return setmode >= 0 && setmode < modesbox->NumEntries(); }, "The set mode is hiogher than the number of modes available!");
	modesbox->SetSelected(setmode);
}

int GUI_ModuleFunction_Thruster::getHeight(int numthrustermodes)
{
	// We'll display up to four thrustermodes without scrolling, plus some space for the group selection plus some padding
	return min(numthrustermodes, 4) * 25 + 150;
}