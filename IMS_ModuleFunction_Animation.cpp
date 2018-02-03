#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Animation.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_Animation_Base.h"
#include "IMS_Animation_Sequential.h"
#include "IMS_Animation_Continuous.h"
#include "IMS_Animation_Tracking.h"
#include "IMS_ModuleFunction_Animation.h"


IMS_ModuleFunction_Animation::IMS_ModuleFunction_Animation(IMS_ModuleFunctionData_Animation *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Base(_data, _module, MTYPE_ANIM), data(_data)
{
	//create the animation instances
	for (UINT i = 0; i < data->animations.size(); ++i)
	{
		IMS_Animation_Base *newanim = NULL;
		//create appropriate animation object
		if (data->animations[i].type == "sequence")
		{
			newanim = new IMS_Animation_Sequential(&data->animations[i]);
		}
		else if (data->animations[i].type == "continuous")
		{
			newanim = new IMS_Animation_Continuous(&data->animations[i]);
		}
		else if (data->animations[i].type.substr(0, 5) == "track")
		{
			newanim = new IMS_Animation_Tracking(&data->animations[i]);
		}
		animations.insert(make_pair(data->animations[i].id, newanim));
	}
}


IMS_ModuleFunction_Animation::~IMS_ModuleFunction_Animation()
{
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		delete i->second;
	}
}


void IMS_ModuleFunction_Animation::AddFunctionToVessel(IMS2 *vessel)
{
	//we're not doing anything here, because animations are a bit peculiar. 
	//The moment this function is called is too early to create animations
	//if the vessel is created at simstart, because the vessel visuals don't
	//exist yet. After integration it is too early, because orbiter didn't 
	//have time yet to properly initialise the mesh. So we're adding the ani-
	//mations by events at another time.
}


void IMS_ModuleFunction_Animation::RemoveFunctionFromVessel(IMS2 *vessel)
{
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		i->second->RemoveAnimationFromVessel();
	}
	animationsexist = false;
}


void IMS_ModuleFunction_Animation::addAnimationsToVessel()
{
	//add all animations to the vessel
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		i->second->AddAnimationToVessel(module->GetVessel(), module->GetMeshIndex(), module->GetRotationMatrix(), module->GetLocalCoordinates());
	}
	animationsexist = true;
}



void IMS_ModuleFunction_Animation::PreStep(double simdt, IMS2 *vessel)
{
	//iterate through animation states
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		//tell all animations to update
		addEvent(i->second->PropagateAnimation(simdt));
	}

	IMS_ModuleFunction_Base::PreStep(simdt, vessel);

}


//event handling
bool IMS_ModuleFunction_Animation::ProcessEvent(Event_Base *e)
{
	if (*e == STARTANIMATIONEVENT)
	{
		//start the animation in question
		StartAnimationEvent *animevent = (StartAnimationEvent*)e;
		map<string, IMS_Animation_Base*>::iterator anim = animations.find(animevent->GetAnimationId());
		if (anim == animations.end())
		{
			Olog::warn("call to nonexisting animation: %s", animevent->GetAnimationId().data());
		}
		else
		{
			//check if the start of the animation is hindered by dependencies
			if (canStart(anim->second, animevent->GetSpeed()))
			{
				addEvent(anim->second->StartAnimation(animevent));
			}
			else
			{
				addEvent(new AnimationFailedEvent(animevent->GetAnimationId(), animevent->GetSpeed()));
			}

		}
		return true;
	}
	else if (*e == MODIFYANIMATIONEVENT)
	{
		ModifyAnimationEvent *animevent = (ModifyAnimationEvent*)e;
		map<string, IMS_Animation_Base*>::iterator anim = animations.find(animevent->GetAnimationId());
		if (anim == animations.end())
		{
			Olog::warn("call to nonexisting animation: %s", animevent->GetAnimationId().data());
		}
		else
		{
			anim->second->ModifyAnimation(animevent);
		}
		return true;
	}
	else if (*e == STOPANIMATIONEVENT)
	{
		//stop the animation in question
		StopAnimationEvent *animevent = (StopAnimationEvent*)e;
		animations.find(animevent->GetAnimationId())->second->StopAnimation();
		return true;
	}
	else if (*e == VESSELVISUALCREATEDEVENT)
	{
		if (!animationsexist)
		{
			addAnimationsToVessel();
		}
		else
		{
			enableAnimations();
		}
	}
	else if (*e == VESSELVISUALDESTROYEDEVENT)
	{
		disableAnimations();
	}
	else if (*e == VESSELLAYOUTCHANGEDEVENT)
	{
		if (module->GetVessel()->GetVisHandle() != NULL && !animationsexist)
		{
			addAnimationsToVessel();
		}
	}


	return false;
}


bool IMS_ModuleFunction_Animation::processScenarioLine(string line)
{
	vector<string> tokens;
	Helpers::Tokenize(string(line), tokens, " ");

	//get the animation in question
	IMS_Animation_Base *anim = animations.find(tokens[0])->second;
	//set the animation state to what's saved in the scenario
	anim->InitStateFromScenario(tokens);
	return true;
}

void IMS_ModuleFunction_Animation::SaveState(FILEHANDLE scn)
{
	//walk through animation and get a state description from everyone
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		string animstate = i->second->GetScenarioLine();
		//check if the state is origin, if not, write to file
		if (animstate != "")
		{
			oapiWriteLine(scn, (char*)animstate.data());
		}
	}
}


void IMS_ModuleFunction_Animation::enableAnimations()
{
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		i->second->EnableAnimation();
	}
}


void IMS_ModuleFunction_Animation::disableAnimations()
{
	for (map<string, IMS_Animation_Base*>::iterator i = animations.begin(); i != animations.end(); ++i)
	{
		i->second->DisableAnimation();
	}
}


bool IMS_ModuleFunction_Animation::canStart(IMS_Animation_Base *anim, double speed)
{
	vector<ANIMATIONDEPENDENCY> deps;
	anim->GetDependencies(deps);
	
	for (UINT i = 0; i < deps.size(); ++i)
	{
		//find the dependency animation
		map<string, IMS_Animation_Base*>::iterator dep = animations.find(deps[i].dependencyid);
		if (dep == animations.end())
		{
			Olog::warn("dependency to nonexisting animation: %s", deps[i].dependencyid.data());
		}
		else
		{
			//check if the dependency applies to the direction the animation should move in
			if (speed > 0 && deps[i].direction > 0 ||
				speed < 0 && deps[i].direction < 0)
			{
				//check if the dependency is in a state where it prevents the animation from starting
				if (dep->second->GetState() != deps[i].dependencystate)
				{
					//if only one dependency doesn't check out, the animation cannot start
					return false;
				}
			}
		}
	}
	return true;
}