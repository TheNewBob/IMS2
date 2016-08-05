#pragma once
/**
 * \brief Notifies that the vessel has lifted off from a planetary surface
 * \see EventHandler.h
 */
class LiftoffEvent :
	public Event_Base
{
public:
	LiftoffEvent() : Event_Base(LIFTOFFEVENT){};
	~LiftoffEvent(){};
};

