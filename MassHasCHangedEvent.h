#pragma once


class MassHasChangedEvent :
	public Event_Base
{
public:
	/**
	* \param delay you can specify a frame delay for this event, though usually it'll be 0.
	*/
	MassHasChangedEvent(int delay = 0) : Event_Base(MASSHASCHANGEDEVENT, delay){};
	~MassHasChangedEvent(){};

};
