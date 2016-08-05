#pragma once

/**
 * \brief Fired when the vessel touches down on a surface
 * \see LiftoffEvent
 * \see EventHandler.h
 */
class TouchdownEvent :
	public Event_Base
{
public:
	TouchdownEvent() : Event_Base(TOUCHDOWNEVENT){};
	~TouchdownEvent(){};
};

