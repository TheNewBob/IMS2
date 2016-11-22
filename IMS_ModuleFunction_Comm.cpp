#include "GuiIncludes.h"
#include "ModuleFunctionIncludes.h"
#include "StateMachineIncludes.h"
#include "IMS_ModuleFunctionData_Comm.h"
#include "GUI_ModuleFunction_Base.h"
#include "GUI_ModuleFunction_Comm.h"
#include "IMS_ModuleFunction_Comm.h"


IMS_ModuleFunction_Comm::IMS_ModuleFunction_Comm(IMS_ModuleFunctionData_Comm *_data, IMS_Module *_module, bool creategui) 
	: IMS_ModuleFunction_Base(_data, _module, MTYPE_COMM), data(_data)
{
	//check which actions are actually supported by the config
	bool hasdeployment = data->deployanimname != "";
	bool hasscanning = data->searchanimname != "";
	bool hastracking = data->trackinganimname != "";

	if (creategui)
	{
		//add our GUI to the module's GUI page
		menu = new GUI_ModuleFunction_Comm(this, module->GetGui(), hasdeployment, hasscanning, hastracking);
	}

	//set up the statemachine
	//create the states the module function can be in

	//deployed is the natural stagin state for everything else. basically,
	//if a module function doesn't support deployment, what it actually doesn't
	//support is retracting. The function will just be regarded as permanently deployed
	state.AddState(COMMSTATE_DEPLOYED, "deployed");

	if (hasdeployment)
	{
		state.AddState(COMMSTATE_RETRACTED, "retracted");
		state.AddState(COMMSTATE_DEPLOYING, "deploying", false);
		state.AddState(COMMSTATE_RETRACTING, "retracting", false);
	}

	if (hasscanning)
	{
		state.AddState(COMMSTATE_SEARCHING, "scanning");
		state.AddState(COMMSTATE_STOP_SEARCHING, "resetting", false);
	}

	if (hastracking)
	{
		state.AddState(COMMSTATE_ALIGNING, "aligning", false);
		state.AddState(COMMSTATE_TRACKING, "tracking");
		state.AddState(COMMSTATE_STOP_TRACKING, "resetting", false);
	}

	//connect the states to create a statemap
	if (hasdeployment)
	{
		state.ConnectStateTo(COMMSTATE_RETRACTED, COMMSTATE_DEPLOYING);			//if antenna retracted, it can move to deploying state
		state.ConnectStateTo(COMMSTATE_DEPLOYING, COMMSTATE_DEPLOYED);			//if antenna deploying, it can move to deployed state
		state.ConnectStateTo(COMMSTATE_DEPLOYED, COMMSTATE_RETRACTING);			//if antenna deployed, it can move to retracting state
		state.ConnectStateTo(COMMSTATE_RETRACTING, COMMSTATE_RETRACTED);		//if antenna retracting, it can move to retracted state
		state.ConnectStateTo(COMMSTATE_RETRACTING, COMMSTATE_DEPLOYING);		//antenna can be retracted while it is deploying
		state.ConnectStateTo(COMMSTATE_DEPLOYING, COMMSTATE_RETRACTING);		//antenna can be deployed while it is retracting
	}

	if (hasscanning)
	{
		state.ConnectStateTo(COMMSTATE_DEPLOYED, COMMSTATE_SEARCHING);			//etc
		state.ConnectStateTo(COMMSTATE_SEARCHING, COMMSTATE_STOP_SEARCHING);
		state.ConnectStateTo(COMMSTATE_STOP_SEARCHING, COMMSTATE_DEPLOYED);
	}

	if (hastracking)
	{
		state.ConnectStateTo(COMMSTATE_DEPLOYED, COMMSTATE_ALIGNING);
		state.ConnectStateTo(COMMSTATE_ALIGNING, COMMSTATE_TRACKING);
		state.ConnectStateTo(COMMSTATE_TRACKING, COMMSTATE_ALIGNING);
		state.ConnectStateTo(COMMSTATE_TRACKING, COMMSTATE_STOP_TRACKING);
		state.ConnectStateTo(COMMSTATE_ALIGNING, COMMSTATE_STOP_TRACKING);		//the aligning state can skip the tracking state and move to the stop tracking state. This is one of the rare occasions where two intermediate states are connected directly
		state.ConnectStateTo(COMMSTATE_STOP_TRACKING, COMMSTATE_DEPLOYED);
		state.ConnectStateTo(COMMSTATE_STOP_TRACKING, COMMSTATE_ALIGNING);
	}
}


IMS_ModuleFunction_Comm::~IMS_ModuleFunction_Comm()
{
}


void IMS_ModuleFunction_Comm::PostLoad()
{
	if (!state.IsInitialised())
	{
		//setting the initial state of the statemachine, as it wasn't loaded from scenario
		if (data->deployanimname != "")
		{
			state.SetInitialState(COMMSTATE_RETRACTED);
		}
		else
		{
			//if deployment is not supported, the antenna is
			//actually considered permanently deployed
			state.SetInitialState(COMMSTATE_DEPLOYED);
		}
		menu->SetStateDescription(state.GetStateDescription());
	}
	else
	{
		//states were loaded from scenario, initialise the control visuals
		//initialise the control visuals
		menu->SetStateDescription(state.GetStateDescription());
		int curstate = state.GetState();

		if (curstate != COMMSTATE_RETRACTED)
		{
			if (curstate == COMMSTATE_DEPLOYING || curstate == COMMSTATE_RETRACTING)
			{
				menu->SetDeployBoxState(BLINKING);
			}
			else
			{
				menu->SetDeployBoxState(ON);
				
				if (curstate == COMMSTATE_SEARCHING)
				{
					menu->SetSearchBoxState(ON);
				}
				else if (curstate == COMMSTATE_TRACKING)
				{
					menu->SetTrackBoxState(ON);
				}
				else
				{
					if (curstate == COMMSTATE_STOP_SEARCHING)
					{
						menu->SetSearchBoxState(BLINKING);
					}
					else if (curstate == COMMSTATE_ALIGNING ||
						curstate == COMMSTATE_STOP_TRACKING)
					{
						menu->SetTrackBoxState(BLINKING);
					}
				}
			}
		}

		int tgtstate = state.GetTargetState();
		if (tgtstate != curstate)
		{
			if (tgtstate == COMMSTATE_RETRACTED)
			{
				menu->SetDeployBoxState(BLINKING);
			}
			else if (tgtstate == COMMSTATE_SEARCHING)
			{
				menu->SetSearchBoxState(BLINKING);
			}
			else if (tgtstate == COMMSTATE_TRACKING)
			{
				menu->SetTrackBoxState(BLINKING);
			}
		}
	}
}


GUI_ModuleFunction_Base *IMS_ModuleFunction_Comm::GetGui()
{ 
	return menu; 
}


void IMS_ModuleFunction_Comm::PreStep(double simdt, IMS2 *vessel)
{
	//don't forget to call the prestep function of the base class!
	IMS_ModuleFunction_Base::PreStep(simdt, vessel);

	
	//check if the state has changed during the last frame,
	//and whether we reached the target state
	if (state.StateChanged())
	{
		//state has changed, update the state description on the gui
		menu->SetStateDescription(state.GetStateDescription());

		//first get the current event. This will immediately advance the state to the next
		//state if it is secure, so don't call GetStateAndAdvance twice in a frame.
		//then take appropriate action depending on what state we ended up in
		//in the case of this module function, this simply takes the form of starting
		//the necessary animations
		switch (state.GetStateAndAdvance())
		{
		case COMMSTATE_DEPLOYING:
			addEvent(new StartAnimationEvent(data->deployanimname, 1.0));
			menu->SetDeployBoxState(BLINKING);
			break;
		case COMMSTATE_RETRACTING:
			addEvent(new StartAnimationEvent(data->deployanimname, -1.0));
			menu->SetDeployBoxState(BLINKING);
			break;
		case COMMSTATE_STOP_SEARCHING:
			addEvent(new StopAnimationEvent(data->searchanimname));
			menu->SetSearchBoxState(BLINKING);
			break;
		case COMMSTATE_STOP_TRACKING:
			addEvent(new StopAnimationEvent(data->trackinganimname));
			menu->SetTrackBoxState(BLINKING);
			break;
		case COMMSTATE_ALIGNING:
			//check if we have a target selected
			if (targetname != "")
			{
				menu->SetTrackBoxState(BLINKING);
				addEvent(new StartTrackingAnimationEvent(data->trackinganimname, 1.0, oapiGetObjectByName((char*)targetname.data())));
			}
			else
			{
				menu->SetTargetDescription("set a valid target for tracking!", true);
				menu->SetTrackBoxState(OFF);
				state.SetTargetState(COMMSTATE_DEPLOYED);
			}
			break;
		case COMMSTATE_SEARCHING:
			addEvent(new StartAnimationEvent(data->searchanimname, 1.0));
			menu->SetSearchBoxState(ON);
			break;
		case COMMSTATE_TRACKING:
			menu->SetTrackBoxState(ON);
			break;
		case COMMSTATE_DEPLOYED:
			menu->SetSearchBoxState(OFF);
			menu->SetTrackBoxState(OFF);
			menu->SetDeployBoxState(ON);
			break;
		case COMMSTATE_RETRACTED:
			menu->SetSearchBoxState(OFF);
			menu->SetTrackBoxState(OFF);
			menu->SetDeployBoxState(OFF);
			break;
		}
	}
}

void IMS_ModuleFunction_Comm::AddFunctionToVessel(IMS2 *vessel)
{

}


void IMS_ModuleFunction_Comm::CommandDeploy()
{
	if (data->deployanimname == "") return;

	if (state.GetState() != COMMSTATE_RETRACTED)
	{
		//if the state is not currently retracted, clicking the deployment box is interpreted as an order to retract
		state.SetTargetState(COMMSTATE_RETRACTED);
		menu->SetDeployBoxState(BLINKING);
	}
	else
	{
		//if it is retracted, we interpret it as an order to deploy
		state.SetTargetState(COMMSTATE_DEPLOYED);
		menu->SetDeployBoxState(BLINKING);
	}
	//if the current state is stable, advance it.
	//if the current state is an Intermediate one, the state
	//will be advanced by an event at the proper time
	state.AdvanceStateSecure();
}


void IMS_ModuleFunction_Comm::CommandSearch()
{
	if (data->searchanimname == "") return;

	//if the comm is currently searching, stop searching
	if (state.GetState() == COMMSTATE_SEARCHING)
	{
		state.SetTargetState(COMMSTATE_DEPLOYED);
		menu->SetSearchBoxState(BLINKING);
	}
	else
	{
		//otherwise, we want to start searching
		state.SetTargetState(COMMSTATE_SEARCHING);
		menu->SetSearchBoxState(BLINKING);
	}
	state.AdvanceStateSecure();
}


void IMS_ModuleFunction_Comm::CommandTrack()
{
	if (data->trackinganimname == "") return;

	int curstate = state.GetState();
	if (curstate == COMMSTATE_TRACKING ||
		curstate == COMMSTATE_ALIGNING) 
	{
		//if the comm is currently tracking or aligning, we want it to stop and get back to deployed state
		state.SetTargetState(COMMSTATE_DEPLOYED);
		menu->SetTrackBoxState(BLINKING);
	}
	else
	{
		state.SetTargetState(COMMSTATE_TRACKING);
		menu->SetTrackBoxState(BLINKING);
	}
	state.AdvanceStateSecure();
}

void IMS_ModuleFunction_Comm::CommandSetTarget()
{
	if (data->trackinganimname == "") return;

	//open an input callback for the user to set the target
	oapiOpenInputBox("enter target", SetTargetInputCallback, NULL, 15, this);
}

void IMS_ModuleFunction_Comm::SetTarget(string target)
{
	if (data->trackinganimname == "") return;

	OBJHANDLE checktarget = oapiGetObjectByName((char*)target.data());
	int currentstate = state.GetState();

	if (checktarget == NULL)
	{
		//if an invalid target was entered, display an error message
		menu->SetTargetDescription("invalid target!", true);
		//if the antenna is currently tracking or aligning with a previous target,
		//stop it and bring it back to origin
		if (currentstate == COMMSTATE_TRACKING ||
			currentstate == COMMSTATE_ALIGNING)
		{
			state.SetTargetState(COMMSTATE_DEPLOYED);
		}
		targetname = "";
	}
	else
	{
		//a valid target was entered, update state, animation and gui
		menu->SetTargetDescription(target);
		targetname = target;
		if (currentstate == COMMSTATE_TRACKING ||
			currentstate == COMMSTATE_ALIGNING)
		{
			addEvent(new ModifyTrackingAnimationEvent(data->trackinganimname, 0.0, checktarget));
			state.SetTargetState(COMMSTATE_TRACKING);
		}
	}
	state.AdvanceStateSecure();
}


bool IMS_ModuleFunction_Comm::ProcessEvent(Event_Base *e)
{ 
	if (*e == ANIMATIONFINISHEDEVENT)
	{
		//the animation that has finished is on of ours, 
		//that means that an intermediate state has reached its end
		//we don't even have to know which one. That's what we have the statemachine for.
		AnimationFinishedEvent *anim = (AnimationFinishedEvent*)e;
		string id = anim->GetAnimationId();
		if (id == data->deployanimname ||
			id == data->searchanimname ||
			id == data->trackinganimname)
		{
			state.AdvanceState();
		}
	}
	else if (*e == ANIMATIONFAILEDEVENT)
	{
		//an animation has failed to start, probably due to an unfulfilled dependency.
		//we need to take action!
		AnimationFailedEvent *anim = (AnimationFailedEvent*)e;
		string id = anim->GetAnimationId();
		int curstate = state.GetState();

		if (id == data->deployanimname)
		{
			//we'll have to know what direction the animation was supposed to move
			if (anim->GetSpeed() < 0 && curstate == COMMSTATE_RETRACTING)
			{
				GUImanager::Alert(data->GetName() + ": unable to retract at the moment!", module->GetGui()->GetFirstVisibleChild());
				state.SetTargetState(COMMSTATE_DEPLOYED);
			}
			else if (anim->GetSpeed() > 0 && curstate == COMMSTATE_DEPLOYING)
			{
				GUImanager::Alert(data->GetName() + ": unable to deploy at the moment!", module->GetGui()->GetFirstVisibleChild());
				state.SetTargetState(COMMSTATE_RETRACTED);
			}
		}
		else if (id == data->searchanimname && curstate == COMMSTATE_SEARCHING)
		{
			GUImanager::Alert(data->GetName() + ": unable to scan at the moment!", module->GetGui()->GetFirstVisibleChild());
			state.SetTargetState(COMMSTATE_DEPLOYED);
		}
		else if (id == data->trackinganimname && curstate == COMMSTATE_ALIGNING)
		{
			GUImanager::Alert(data->GetName() + ": unable to start tracking at the moment!", module->GetGui()->GetFirstVisibleChild());
			state.SetTargetState(COMMSTATE_DEPLOYED);
		}
		state.AdvanceStateSecure();
	}
	else if (*e == ANIMATIONSTARTEDEVENT)
	{
		//an animation has been started. Probably it was started by this module function, in which case we don't need to do anything.
		//but if module functions have shared animations, the state of this module function must now change.
		AnimationStartedEvent *ev = (AnimationStartedEvent*)e;
		string animid = ev->GetAnimationId();
		int curstate = state.GetState();
		if (animid == data->deployanimname)
		{
			//check if the started animation concerns this function, and whether it needs to change state as a result
			if (ev->GetSpeed() > 0 && curstate == COMMSTATE_RETRACTED)
			{
				state.SetTargetState(COMMSTATE_DEPLOYED);
				state.AdvanceStateSecure();
			}
			else if (ev->GetSpeed() < 0 && curstate == COMMSTATE_DEPLOYED)
			{
				state.SetTargetState(COMMSTATE_RETRACTED);
				state.AdvanceStateSecure();
			}
		}
		else if (animid == data->searchanimname && curstate == COMMSTATE_DEPLOYED)
		{
			state.SetTargetState(COMMSTATE_SEARCHING);
			state.AdvanceStateSecure();
		}
		else if (animid == data->trackinganimname && curstate == COMMSTATE_DEPLOYED)
		{
			state.SetTargetState(COMMSTATE_TRACKING);
			state.AdvanceStateSecure();
		}
	}
	else if (*e == TRACKINGANIMATIONSTATUSEVENT)
	{
		//while the array is tracking, we get a status update every frame.
		//Currently, we're only interested if the array is aligning or aligned
		TrackingAnimationStatusEvent *status = (TrackingAnimationStatusEvent*)e;
		//check if the status update is actually for this modulefunction, and whether it has changed
		if (status->GetAnimationId() == data->trackinganimname && status->GetStatus() != lasttrackingstatus)
		{
			//if the antenna was unable to track the target before,
			//we have to update the state display to make the error message disapear
			if (lasttrackingstatus == UNABLE)
			{
				menu->SetStateDescription(state.GetStateDescription());
			}

			if (status->GetStatus() == ALIGNED)
			{
				if (state.GetState() == COMMSTATE_ALIGNING)
				{
					//the array has just aligned itself with the target, update the statemachine
					state.AdvanceState();
				}
			}
			else if (status->GetStatus() == ALIGNING)
			{
				if (state.GetState() == COMMSTATE_TRACKING)
				{
					state.SetTargetState(COMMSTATE_TRACKING);
				}
			}
			else if (status->GetStatus() == REVERTING)
			{
				if (state.GetState() == COMMSTATE_ALIGNING ||
					state.GetState() == COMMSTATE_TRACKING)
				{
					//the array is on its way back to deployed position. Stop whatever it is it's doing right now
					state.AdvanceState();
				}
			}
			else if (status->GetStatus() == UNABLE)
			{
				//the array is unable to align with the specified target.
				//print an error message to the gui.
				menu->SetStateDescription("unable to align with " + targetname + "!", true);
			}
			lasttrackingstatus = status->GetStatus();
		}
	}
	
	return false; 
}



void IMS_ModuleFunction_Comm::SaveState(FILEHANDLE scn)
{
	int curstate, tgtstate;
	curstate = state.GetState();
	tgtstate = state.GetTargetState();
	oapiWriteScenario_int(scn, "STATE", curstate);
	if (tgtstate != curstate)
	{
		oapiWriteScenario_int(scn, "TGTSTATE", state.GetTargetState());
	}
	if (targetname != "")
	{
		//there is a tracking target set, so save that too
		oapiWriteScenario_string(scn, "TRACKINGTGT", (char*)targetname.data());
	}
}



bool IMS_ModuleFunction_Comm::processScenarioLine(string line)
{
	if (line.substr(0, 5) == "STATE")
	{
		int curstate = atoi(line.substr(6).data());
		state.SetInitialState(curstate);
		return true;
	}
	else if (line.substr(0, 8) == "TGTSTATE")
	{
		int tgtstate = atoi(line.substr(9).data());
		if (tgtstate != state.GetState())
		{
			state.SetTargetState(tgtstate);
		}
		return true;
	}
	else if (line.substr(0, 11) == "TRACKINGTGT")
	{
		targetname = line.substr(12);
		menu->SetTargetDescription(targetname);
		return true;
	}
	return false;
}

bool SetTargetInputCallback(void *id, char *str, void *usrdata)
{
	if (strlen(str) == 0)
	{
		return false;
	}

	IMS_ModuleFunction_Comm *comm = (IMS_ModuleFunction_Comm*)usrdata;
	comm->SetTarget(string(str));
	return true;
}


