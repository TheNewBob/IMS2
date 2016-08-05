#pragma once

/**
* \brief Notifies that the vessel's visuals have been destroyed
*
* Triggered by clbkVisualDestroyed()
* \see VesselVisualCreatedEvent
* \see EventHandler.h
*/
class VesselVisualDestroyedEvent :
	public Event_Base
{
public:
	VesselVisualDestroyedEvent() : Event_Base(VESSELVISUALDESTROYEDEVENT, 0){};
	~VesselVisualDestroyedEvent(){};

private:
};