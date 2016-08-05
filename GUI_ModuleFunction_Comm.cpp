#include "GUI_Common.h"
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


GUI_ModuleFunction_Comm::GUI_ModuleFunction_Comm(IMS_ModuleFunction_Comm *_function, GUIplugin *gui, bool hasdeployment, bool hasscanning, bool hastracking)
	: GUI_ModuleFunction_Base(getHeight(hasdeployment, hasscanning, hastracking), gui, gui->GetStyle(STYLE_BUTTON)), function(_function)
{
	//create the required controls
	gui->CreateLabel(function->GetData()->GetName(), _R(10, 10, width - 10, 35), id);

	//the statedescription will always be there
	statedescription = gui->CreateLabelValuePair("Status:", "", _R(120, 40, 400, 65), id, -1, STYLE_DEFAULT, GUI_SMALL_ERROR_FONT);

	int curypos = 40;
	if (hasdeployment)
	{
		deploybox = gui->CreateCheckBox("deploy", _R(15, curypos, 120, curypos + 25), id);
		curypos += 30;
	}

	if (hasscanning)
	{
		searchbox = gui->CreateCheckBox("scan", _R(15, curypos, 120, curypos + 25), id);
		curypos += 30;
	}

	if (hastracking)
	{
		//now things are getting a bit more complicated, because tracking controll needs 3 gui elements to work, 
		//resulting in three possible layouts depending on what other controls are supported

		trackbox = gui->CreateCheckBox("track", _R(15, curypos, 120, curypos + 25), id);

		if (curypos == 40)
		{
			//tracking is all there is. draw the button and the indicator on the line below, side by side
			//the gui will look like this:
			//trackbox		statedescription
			//targetbtn		targetdescription
			settargetbtn = gui->CreateDynamicButton("set target", _R(15, 70, 95, 95), id);
			targetdescription = gui->CreateLabelValuePair("Target:", "None", _R(120, 70, 400, 95), id, -1, STYLE_DEFAULT, GUI_SMALL_ERROR_FONT);
		}
		else if (curypos == 70)
		{
			//the upper line is taken by either the deploybox or the searchbox. set the gui up like this:
			//whateverbox		statedescription
			//trackbox
			//targetbtn			targetdescription
			settargetbtn = gui->CreateDynamicButton("set target", _R(120, 100, 200, 125), id);
			targetdescription = gui->CreateLabelValuePair("Target:", "None", _R(120, 100, 400, 125), id, -1, STYLE_DEFAULT, GUI_SMALL_ERROR_FONT);
		}
		else
		{
			//all operations are supported, set the GUI up like this:
			//deploybox			statedescription
			//searchbox			targetdescription
			//trackbox			targetbtn
			targetdescription = gui->CreateLabelValuePair("Target:", "None", _R(120, 70, 400, 95), id, -1, STYLE_DEFAULT, GUI_SMALL_ERROR_FONT);
			settargetbtn = gui->CreateDynamicButton("set target", _R(120, 100, 200, 125), id);
		}
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

int GUI_ModuleFunction_Comm::getHeight(bool hasdeployment, bool hasscanning, bool hastracking)
{
	if (!hastracking) 
	{
		//there are no tracking controls. this means the interface consists of either one or two lines,
		//depending on what else there is
		if (hasscanning && hasdeployment)
		{
			//both deployment and scanning controls are present, we have two lines
			return 100;
		}
		else
		{
			//only one line
			return 70;
		}
	}
	else if (!hasscanning && !hasdeployment)
	{
		//tracking controls are the only controls there are. we need 2 lines
		return 100;
	}

	//any other combination, we need 3 lines
	return 130;
}










