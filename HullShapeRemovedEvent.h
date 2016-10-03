#pragma once
/**
* \brief Notifies that a hullshape has been removed from the vessels collision mesh
* \see EventHandler.h
*/
class HullShapeRemovedEvent :
	public Event_Base
{
public:
	HullShapeRemovedEvent() : Event_Base(HULLSHAPEREMOVEDEVENT){};
	~HullShapeRemovedEvent(){};
};