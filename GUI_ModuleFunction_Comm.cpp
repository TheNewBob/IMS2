#include "GUI_Common.h"
#include "Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "Events.h"
#include <deque>
#include "SimpleTreeNode.h"
#include "SimplePathNode.h"
#include "StateNode.h"
#include "StateMachine.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Comm.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Comm.h"
#include "GUI_ModuleFunction_Comm.h"
#include "IMS_InputCallback.h"
#include "LayoutManager.h"

const static string FILENAME = "modulefunctions/comm.xml";
const static string NAME_LABEL = "name_label";
const static string STATUS = "status";
const static string DEPLOY_CHKBX = "deploy_chkbx";
const static string SCAN_CHKBX = "scan_chkbx";
const static string TRACK_CHKBX = "track_chkbx";
const static string SET_TGT_BTN = "set_tgt_btn";
const static string TGT_DESCRIPTION = "tgt_description";

GUI_ModuleFunction_Comm::GUI_ModuleFunction_Comm(IMS_ModuleFunction_Comm *_function, GUIplugin *gui, bool hasdeployment, bool hasscanning, bool hastracking)
	: GUI_ModuleFunction_Base(getHeight(hasdeployment, hasscanning, hastracking, predictWidth(gui, INT_MAX)), gui, gui->GetStyle(STYLE_BUTTON)), function(_function)
{
	//compose a list of fields we don't need for this instance, because the functionality is not supported.
	vector<string> ignore_fields;
	makeIgnoredFields(ignore_fields, hasdeployment, hasscanning, hastracking);
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(FILENAME);

	//create the required controls
	gui->CreateLabel(function->GetData()->GetName(), getElementRect(NAME_LABEL, l, ignore_fields), id);

	//the statedescription will always be there
	statedescription = gui->CreateLabelValuePair("Status:", "", getElementRect(STATUS, l, ignore_fields), id, -1, STYLE_DEFAULT, "small-error-font");

	if (hasdeployment)
	{
		deploybox = gui->CreateCheckBox("deploy", getElementRect(DEPLOY_CHKBX, l, ignore_fields), id);
	}

	if (hasscanning)
	{
		searchbox = gui->CreateCheckBox("scan", getElementRect(SCAN_CHKBX, l, ignore_fields), id);
	}

	if (hastracking)
	{
		trackbox = gui->CreateCheckBox("track", getElementRect(TRACK_CHKBX, l, ignore_fields), id);
		settargetbtn = gui->CreateDynamicButton("set target", getElementRect(SET_TGT_BTN, l, ignore_fields), id);
		targetdescription = gui->CreateLabelValuePair("Target:", "None", getElementRect(TGT_DESCRIPTION, l, ignore_fields), id, -1, STYLE_DEFAULT, "small-error-font");
	}
}


GUI_ModuleFunction_Comm::~GUI_ModuleFunction_Comm()
{

}





int GUI_ModuleFunction_Comm::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//send the event to the children, so they can check if they were clicked
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	//no children of this element have been clicked
	if (eventId == -1) return -1;

	//if we get until here, it means a child of this element has indeed been clicked
	//By implementing the eventhandling here, we're effectively intercepting that event before it 
	//goes further down the hierarchy. We'll still pass it downwards at the end, because at the lowest level
	//(the GUImanager) it will trigger the redraw of the panel element containing this Element. But here's the
	//only place where we can propperly decide what the event actually means.
	if (deploybox && eventId == deploybox->GetId())
	{
		function->CommandDeploy();
	}
	else if (searchbox && eventId == searchbox->GetId())
	{
		function->CommandSearch();
	}
	else if (trackbox && eventId == trackbox->GetId())
	{
		function->CommandTrack();
	}
	else if(settargetbtn && eventId == settargetbtn->GetId())
	{
		function->CommandSetTarget();
	}

	return eventId;
}



bool GUI_ModuleFunction_Comm::updateMe()
{
	bool needsupdate = false;

	if (updatenextframe)
	{
		//the module has reached a stable state, update the gui visuals immediately
		updatenextframe = false;
		needsupdate = true;
	}

	//if any of the checkboxes are blinking, update them every second
	if (GUImanager::CheckSynchronisedInterval(1000))
	{
		if (deployboxstate == BLINKING)
		{
			deploybox->ToggleChecked();
			needsupdate = true;
		}
		if (searchboxstate == BLINKING)
		{
			searchbox->ToggleChecked();
			needsupdate = true;
		}
		if (trackboxstate == BLINKING)
		{
			trackbox->ToggleChecked();
			needsupdate = true;
		}
	}
	return needsupdate;
}


void GUI_ModuleFunction_Comm::SetDeployBoxState(CHECKBOXSTATE newstate)
{ 
	if (newstate != deployboxstate)
	{
		//if a checkbox is set to on or off, this means that
		//the module has reached a stable state. We have to redraw the gui visuals
		//to reflect that.
		if (newstate != BLINKING)
		{
			updatenextframe = true;
		}

		deployboxstate = newstate;
		switch (deployboxstate)
		{
		case OFF:
			deploybox->SetChecked(false);
			break;
		case ON:
			deploybox->SetChecked(true);
			break;
		}
	}
}


void GUI_ModuleFunction_Comm::SetSearchBoxState(CHECKBOXSTATE newstate)
{
	if (newstate != searchboxstate)
	{
		//if a checkbox is set to on or off, this means that
		//the module has reached a stable state. We have to redraw the gui visuals
		//to reflect that.
		if (newstate != BLINKING)
		{
			updatenextframe = true;
		}

		searchboxstate = newstate;
		switch (searchboxstate)
		{
		case OFF:
			searchbox->SetChecked(false);
			break;
		case ON:
			searchbox->SetChecked(true);
			break;
		}
	}
}


void GUI_ModuleFunction_Comm::SetTrackBoxState(CHECKBOXSTATE newstate)
{
	if (newstate != trackboxstate)
	{
		//if a checkbox is set to on or off, this means that
		//the module has reached a stable state. We have to redraw the gui visuals
		//to reflect that.
		if (newstate != BLINKING)
		{
			updatenextframe = true;
		}

		trackboxstate = newstate;
		switch (trackboxstate)
		{
		case OFF:
			trackbox->SetChecked(false);
			break;
		case ON:
			trackbox->SetChecked(true);
			break;
		}
	}
}

void GUI_ModuleFunction_Comm::SetStateDescription(string state, bool hilighted)
{
	if (state != statedescription->GetValue())
	{
		statedescription->SetValue(state, hilighted);
		updatenextframe = true;
	}
}

void GUI_ModuleFunction_Comm::SetTargetDescription(string target, bool hilighted)
{
	if (target != targetdescription->GetValue())
	{
		targetdescription->SetValue(target, hilighted);
		updatenextframe = true;
	}
}

int GUI_ModuleFunction_Comm::getHeight(bool hasdeployment, bool hasscanning, bool hastracking, int width)
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(FILENAME);
	GUI_Layout *usedlayout = l->GetLayoutForWidth(width);
	
	vector<string> ignored_fields;
	makeIgnoredFields(ignored_fields, hasdeployment, hasscanning, hastracking);
	return usedlayout->GetLayoutHeight(ignored_fields);
}


void GUI_ModuleFunction_Comm::makeIgnoredFields(vector<string> &OUT_ignored_fields, bool hasdeployment, bool hasscanning, bool hastracking)
{
	if (!hastracking)
	{
		OUT_ignored_fields.push_back(TRACK_CHKBX);
		OUT_ignored_fields.push_back(SET_TGT_BTN);
		OUT_ignored_fields.push_back(TGT_DESCRIPTION);
	}

	if (!hasscanning)
	{
		OUT_ignored_fields.push_back(SCAN_CHKBX);
	}

	if (!hasdeployment)
	{
		OUT_ignored_fields.push_back(DEPLOY_CHKBX);
	}
}








