#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Animation.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Animation_Base.h"
#include "IMS_Animation_Sequential.h"


IMS_Animation_Sequential::IMS_Animation_Sequential(ANIMATIONDATA *_data) : IMS_Animation_Base(_data)
{
}


IMS_Animation_Sequential::~IMS_Animation_Sequential()
{
}


AnimationEvent_Base *IMS_Animation_Sequential::PropagateAnimation(double simdt)
{


	//check if the animation is running at all
	if (speed != 0.0)
	{
		//calculate how much the state propagates in the passed time and update it
		//this is where the abstract speed value becomes an actual speed in combination with the animation's default duration
		//if speed is 1 or -1, the animation will need duration seconds to complete. 
		state += (float)(simdt / data->duration) * speed;

		//check if the animation has finished or not, in both directions
		if (state <= 0 || state >= 1)
		{
			//set the state to its boundaries and stop the animation
			state = max(state, 0);
			state = min(state, 1);
			speed = 0.0;
		}

	
		//set the state of the animation in orbiter if the visual exists
		if (enabled)
		{
			vessel->SetAnimation(orbiterid, state);
		}
		
		//if the animation has finished, return the event.
		if (speed == 0.0)
		{
			return new AnimationFinishedEvent(data->id);
		}
	}

	return NULL;
}


