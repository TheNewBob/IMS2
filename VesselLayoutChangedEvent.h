#pragma once
//event that signifies a change in the vessel layout, i.e. modules were added or removed

/**
 * \brief Fired whenever the integrated vessel structure has changed
 *
 * In explicit, it fires when modules are assembled or split
 * \see EventHandler.h
 */
class VesselLayoutChangedEvent :
	public Event_Base
{
public:
	VesselLayoutChangedEvent() : Event_Base(VESSELLAYOUTCHANGEDEVENT, 1){};
	~VesselLayoutChangedEvent(){};

private:
};
