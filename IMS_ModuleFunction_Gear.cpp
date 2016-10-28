#include "Common.h"
#include "Managers.h"
#include "IMS_TouchdownPointManager.h"
#include "IMS.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Gear.h"
#include "StateMachineIncludes.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Gear.h"

IMS_ModuleFunction_Gear::IMS_ModuleFunction_Gear(IMS_ModuleFunctionData_Gear *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Base(_data, _module, MTYPE_GEAR), data(_data)
{
	// deployed is the natural staging state for everything else. basically,
	//if a module function doesn't support deployment, what it actually doesn't
	//support is retracting. The function will just be regarded as permanently deployed
	state.AddState(GEARSTATE::DEPLOYED, "deployed");

	//add the rest of the states if they are supported
	if (data->candeploy)
	{
		state.AddState(GEARSTATE::RETRACTED, "retracted");
		state.AddState(GEARSTATE::DEPLOYING, "deploying", false);
		state.AddState(GEARSTATE::RETRACTING, "retracting", false);
	
		//connect the states
		state.ConnectStateTo(GEARSTATE::RETRACTED, GEARSTATE::DEPLOYING);		//if gear retracted, it can move to deploying state
		state.ConnectStateTo(GEARSTATE::DEPLOYING, GEARSTATE::DEPLOYED);		//if gear deploying, it can move to deployed state
		state.ConnectStateTo(GEARSTATE::DEPLOYED, GEARSTATE::RETRACTING);		//if gear deployed, it can move to retracting state
		state.ConnectStateTo(GEARSTATE::RETRACTING, GEARSTATE::RETRACTED);		//if gear retracting, it can move to retracted state
		state.ConnectStateTo(GEARSTATE::RETRACTING, GEARSTATE::DEPLOYING);		//gear can be retracted while it is deploying
		state.ConnectStateTo(GEARSTATE::DEPLOYING, GEARSTATE::RETRACTING);		//gear can be deployed while it is retracting
	}
}


IMS_ModuleFunction_Gear::~IMS_ModuleFunction_Gear()
{
}


void IMS_ModuleFunction_Gear::PostLoad()
{
	if (!state.IsInitialised())
	{
		//setting the initial state of the statemachine, as it wasn't loaded from scenario
		if (data->candeploy)
		{
			state.SetInitialState(GEARSTATE::RETRACTED);
		}
		else
		{
			//if deployment is not supported, the gear is
			//actually considered permanently deployed
			state.SetInitialState(GEARSTATE::DEPLOYED);
		}
	}
}


void IMS_ModuleFunction_Gear::AddFunctionToVessel(IMS2 *vessel)
{
	tdpmanager = vessel->GetTdPointManager();
	if (state.GetState() == GEARSTATE::DEPLOYED)
	{
		addTdPointToVessel();
	}
}


void IMS_ModuleFunction_Gear::RemoveFunctionFromVessel(IMS2 *vessel)
{
	removeTdPointFromVessel();
}


void IMS_ModuleFunction_Gear::addTdPointToVessel()
{
	//only add the point if it wasn't added
	if (tdpoint_id == 0)
	{
		VECTOR3 rotatedpos = mul(module->GetRotationMatrix(), data->touchdownpoint);
		VECTOR3 modulepos;
		module->GetPos(modulepos);
		tdpoint_id = tdpoint_id = tdpmanager->AddLandingTdPoint(rotatedpos + modulepos);
	}
}


void IMS_ModuleFunction_Gear::removeTdPointFromVessel()
{
	//only remove the point if it was actually added.
	if (tdpoint_id != 0)
	{
		tdpmanager->RemoveLandingTdPoint(tdpoint_id);
	}
	tdpoint_id = 0;
}


void IMS_ModuleFunction_Gear::PreStep(double simdt, IMS2 *vessel)
{
	IMS_ModuleFunction_Base::PreStep(simdt, vessel);

	//check if the state has changed during the last frame,
	//and whether we reached the target state
	if (state.StateChanged())
	{
		switch (state.GetStateAndAdvance())
		{
		case GEARSTATE::DEPLOYING:
			addEvent(new StartAnimationEvent(data->deployanimname, 1.0));
			break;
		case GEARSTATE::RETRACTING:
			addEvent(new StartAnimationEvent(data->deployanimname, -1.0));
			removeTdPointFromVessel();
			break;
		case GEARSTATE::DEPLOYED:
			addTdPointToVessel();
			break;
		}
	}
}


bool IMS_ModuleFunction_Gear::ProcessEvent(Event_Base *e)
{
	if (*e == TOGGLEGEAREVENT && data->candeploy)
	{
		int curstate = state.GetState();
		//somnebody wants to toggle the gear...
		if (curstate == GEARSTATE::DEPLOYED)
		{
			state.SetTargetState(GEARSTATE::RETRACTED);
		}
		else if (curstate == GEARSTATE::DEPLOYING)
		{
			addEvent(new StopAnimationEvent(data->deployanimname));
			state.SetTargetState(GEARSTATE::RETRACTED);
		}
		else if (curstate == GEARSTATE::RETRACTED)
		{
			state.SetTargetState(GEARSTATE::DEPLOYED);
		}
		else if (curstate == GEARSTATE::RETRACTING)
		{
			addEvent(new StopAnimationEvent(data->deployanimname));
			state.SetTargetState(GEARSTATE::DEPLOYED);
		}
		state.AdvanceStateSecure();
	}
	else if (*e == ANIMATIONFINISHEDEVENT)
	{
		//an animation has finished, check if it's ours
		AnimationFinishedEvent *anim = (AnimationFinishedEvent*)e;
		string id = anim->GetAnimationId();
		if (id == data->deployanimname)
		{
			state.AdvanceState();
		}
	}
	else if (*e == ANIMATIONSTARTEDEVENT)
	{
		//an animation has started, check if it's one of ours
		AnimationStartedEvent *ev = (AnimationStartedEvent*)e;
		string animid = ev->GetAnimationId();
		int curstate = state.GetState();
		if (animid == data->deployanimname)
		{
			//check if the started animation concerns this function, and whether it needs to change state as a result
			if (ev->GetSpeed() > 0 && curstate == GEARSTATE::RETRACTED)
			{
				state.SetTargetState(GEARSTATE::DEPLOYED);
				state.AdvanceStateSecure();
			}
			else if (ev->GetSpeed() < 0 && curstate == GEARSTATE::DEPLOYED)
			{
				state.SetTargetState(GEARSTATE::RETRACTED);
				state.AdvanceStateSecure();
			}
		}
	}

	return false;
}


IMS_Module *IMS_ModuleFunction_Gear::GetModule()
{
	return module;
}


void IMS_ModuleFunction_Gear::SaveState(FILEHANDLE scn)
{
	int curstate, tgtstate;
	curstate = state.GetState();
	tgtstate = state.GetTargetState();
	oapiWriteScenario_int(scn, "STATE", curstate);
	if (tgtstate != curstate)
	{
		oapiWriteScenario_int(scn, "TGTSTATE", state.GetTargetState());
	}
}


bool IMS_ModuleFunction_Gear::processScenarioLine(string line)
{
	if (line.substr(0, 5) == "STATE")
	{
		int curstate = atoi(line.substr(6).data());
		state.SetInitialState(curstate);
		return true;
	}
	return false;
}