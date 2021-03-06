#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Animation.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Animation_Base.h"
#include "Calc.h"


IMS_Animation_Base::IMS_Animation_Base(ANIMATIONDATA *_data)
	: data(_data), state(0.0), speed(0)
{
}


IMS_Animation_Base::~IMS_Animation_Base()
{
}


void IMS_Animation_Base::InitStateFromScenario(vector<string> &line)
{
	if (line.size() != 3)
	{
		//invalid line. No need to terminate anything, the animation will just be reset to origin
		Helpers::writeToLog(string("Invalid state for animation in scenario!"), L_WARNING);
	}
	else
	{
		//set the animation state to what's saved in the scenario
		state = atof(line[1].data());
		speed = atof(line[2].data());
	}
}


void IMS_Animation_Base::AddAnimationToVessel(IMS2 *_vessel, int _meshindex, MATRIX3 moduleorientation, VECTOR3 modulelocalpos)
{

	vessel = _vessel;
	meshindex = _meshindex;

	//create animation on vessel and remember the orbiter ID 
	orbiterid = vessel->CreateAnim(0.0);
	Helpers::writeToLog(string("created animation " + data->id + " " + Helpers::intToString(orbiterid)), L_DEBUG);

	//create the animation on the vessel. We need to store the handles for parent-child relations
	//and for proper deletion later on.
	for (UINT i = 0; i < data->components.size(); ++i)
	{
		ANIMATIONCOMPONENT_HANDLE parent = NULL;
		int parentidx = data->components[i].parent;

		//check if the component has a parent
		if (parentidx > -1)
		{
			//sanity check in case the writer of the config file screwed up and declared a wrong order
			if (parentidx >= (int)animationcomponents.size())
			{
				Helpers::writeToLog(string("parent-child animations must be declared in order!"), L_ERROR);
				return;
			}
			parent = animationcomponents[parentidx];
		}

		if (data->components[i].type == "rotate")
		{
			animationcomponents.push_back(vessel->AddAnimationComponent(orbiterid, data->components[i].duration[0], data->components[i].duration[1],
				createRotationComponent(&data->components[i], modulelocalpos, moduleorientation),
				parent));
		}
		else if (data->components[i].type == "translate")
		{
			animationcomponents.push_back(vessel->AddAnimationComponent(orbiterid, data->components[i].duration[0], data->components[i].duration[1],
				createTranslationComponent(&data->components[i], modulelocalpos, moduleorientation),
				parent));
		}
		else if (data->components[i].type == "scale")
		{
			animationcomponents.push_back(vessel->AddAnimationComponent(orbiterid, data->components[i].duration[0], data->components[i].duration[1],
				createScaleComponent(&data->components[i], modulelocalpos, moduleorientation),
				parent));
		}
	}
	EnableAnimation();
}


void IMS_Animation_Base::RemoveAnimationFromVessel()
{
	Helpers::writeToLog(string("Deleting animation " + data->id + " " + Helpers::intToString(orbiterid)), L_DEBUG);

	DisableAnimation();
	//delete the animationcomponents in reverse order.
	//technically DelAnimation should do that, but it 
	//seems it doesn't get the order right and crashes.
	for (UINT i = animationcomponents.size(); i > 0; --i)
	{
		bool result = vessel->DelAnimationComponent(orbiterid, animationcomponents[i - 1]);
		assert(result);
	}
	animationcomponents.clear();
	bool result = vessel->DelAnimation(orbiterid);
//	assert(result);

	//delete the meshgroup identifiers
	//for some reason, deleting them before the animation can cause a crash.
	for (UINT i = 0; i < animationgroups.size(); ++i)
	{
		delete[] animationgroups[i];
	}
	animationgroups.clear();
	orbiterid = -1;

}



MGROUP_ROTATE *IMS_Animation_Base::createRotationComponent(ANIMCOMPONENTDATA *comp, VECTOR3 modulelocalpos, MATRIX3 moduleorientation)
{
	//create an array for the groups and copy them over
	UINT *groups = new UINT[comp->groups.size()];
	for (UINT j = 0; j < comp->groups.size(); ++j)
	{
		groups[j] = comp->groups[j];
	}
	animationgroups.push_back(groups);

	//get the rotation reference and transform it to the modules alignement within the vessel
	VECTOR3 rotreference = comp->reference;
	rotreference = mul(moduleorientation, rotreference);
	Calc::RoundVector(rotreference, 1000);
//	rotreference += modulelocalpos;

	//doing the same with the rotation axis
	VECTOR3 rotaxis = comp->axis;
	rotaxis = mul(moduleorientation, rotaxis);
	normalise(rotaxis);


	//bunch it all together into an MGROUP_ROTATE object
	return new MGROUP_ROTATE(
		meshindex,
		groups, comp->groups.size(),
		rotreference,
		rotaxis,
		(float)RAD_PER_DEGREE * comp->range);
}



MGROUP_TRANSLATE *IMS_Animation_Base::createTranslationComponent(ANIMCOMPONENTDATA *comp, VECTOR3 modulelocalpos, MATRIX3 moduleorientation)
{
	//create an array for the groups and copy them over
	UINT *groups = new UINT[comp->groups.size()];
	for (UINT j = 0; j < comp->groups.size(); ++j)
	{
		groups[j] = comp->groups[j];
	}
	animationgroups.push_back(groups);

	//rotate the translation vector to the module's alignement
	VECTOR3 translation = comp->reference;
	translation = mul(moduleorientation, translation);
	Calc::RoundVector(translation, 1000);


	//bunch it all together into an MGROUP_ROTATE object
	return new MGROUP_TRANSLATE(
		meshindex,
		groups, comp->groups.size(),
		translation);
}



MGROUP_SCALE *IMS_Animation_Base::createScaleComponent(ANIMCOMPONENTDATA *comp, VECTOR3 modulelocalpos, MATRIX3 moduleorientation)
{
	//create an array for the groups and copy them over
	UINT *groups = new UINT[comp->groups.size()];
	for (UINT j = 0; j < comp->groups.size(); ++j)
	{
		groups[j] = comp->groups[j];
	}
	animationgroups.push_back(groups);

	//get the scaling origin and transform it to the modules alignement within the vessel
	VECTOR3 origin = comp->reference;
	origin = mul(moduleorientation, origin);
	Calc::RoundVector(origin, 1000);

	//doing the same with the actual scale factor (axis doubles for scale at this point in the ANIMATIONCOMPONENT struct)
	VECTOR3 scale = comp->axis;
	scale = mul(moduleorientation, scale);
	Calc::RoundVector(scale, 1000);


	//bunch it all together into an MGROUP_ROTATE object
	return new MGROUP_SCALE(
		meshindex,
		groups, comp->groups.size(),
		origin,
		scale);

}



AnimationEvent_Base *IMS_Animation_Base::StartAnimation(StartAnimationEvent *startevent)
{
	//check if the animation isn't running already
	if (speed == 0)
	{
		double _speed = startevent->GetSpeed();
		//check if the passed speed is within bounds
		if ((_speed < 0.0 || _speed > 0.0) &&
			abs(_speed) <= maxspeed)
		{
			//make the animation run at the specified speed in the specifeid direction
			speed = _speed;
		}
		else if (_speed == 0.0)
		{
			//start animation at default speed if it's not at the end. Otherwise, start it in reverse
			if (state < 1.0)
			{
				speed = 1.0;
			}
			else
			{
				speed = -1.0;
			}
		}
		return new AnimationStartedEvent(data->id, speed);
	}
	return NULL;
}


void IMS_Animation_Base::ModifyAnimation(ModifyAnimationEvent *modifyevent)
{
	double _speed = modifyevent->GetSpeed();
	//check if the passed speed is within bounds
	if ((_speed < 0.0 || _speed > 0.0) &&
		abs(_speed) <= maxspeed)
	{
		//make the animation run at the specified speed in the specifeid direction
		speed = _speed;
	}
}


void IMS_Animation_Base::StopAnimation()
{
	speed = 0;
}

string IMS_Animation_Base::GetScenarioLine()
{
	//if the animation is at origin, we don't need to save it to the scenario
	if (state == 0.0 && speed == 0.0)
	{
		return "";
	}

	//otherwise we need the id plus the current state and speed
	stringstream ss;
	ss << data->id << " " << state << " " << speed;
	return ss.str();
}


void IMS_Animation_Base::DisableAnimation()
{
	//make sure the animation actually exists on the vessel
	if (animationcomponents.size() > 0)
	{
		vessel->SetAnimation(orbiterid, 0.0);
		enabled = false;
	}
}

void IMS_Animation_Base::EnableAnimation()
{
	//make sure the animation actually exists on the vessel
	if (animationcomponents.size() > 0)
	{
		vessel->SetAnimation(orbiterid, state);
		enabled = true;
	}
}