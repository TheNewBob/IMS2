#pragma once
/**
* \brief Notifies that a hullshape has been removed from the vessels collision mesh
* \see EventHandler.h
*/
class ToggleGearEvent :
	public Event_Base
{
public:
	ToggleGearEvent() : Event_Base(TOGGLEGEAREVENT, 1){};
	~ToggleGearEvent(){};
};