#pragma once

/**
 * \brief Notifies that the vessel's visuals have been created
 *
 * Triggered by clbkVisualCreated()
 * \see VesselVisualDestroyedEvent
 * \see EventHandler.h
 */
class VesselVisualCreatedEvent :
	public Event_Base
{
public:
	VesselVisualCreatedEvent() : Event_Base(VESSELVISUALCREATEDEVENT, 0){};
	~VesselVisualCreatedEvent(){};

private:
};