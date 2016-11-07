#pragma once

/**
* \brief Sets the current state of an animation that is not currently running.
* \note This event has NO effect on animations that are currently running, and you won't get a response in either case.
*
* \see StartAnimationEvent
* \see StopAnimationEvent
* \see EventHandler.h
*/
class SetAnimationStateEvent :
	public AnimationEvent_Base
{
public:
	/**
	* \param animationid Identifier of the animation this event is aimed at
	* \param state The state an animation should be set to, between 0 and 1.
	*/
	SetAnimationStateEvent(string animationid, double state) : AnimationEvent_Base(SETANIMATIONSTATEEVENT, animationid, 0),
		state(state){};
	~SetAnimationStateEvent(){};

	/**
	* \return The state the animation should be set to.
	*/
	double GetState() { return state; };

protected:

	double state;			//!< the state an animation should be set to.
};
