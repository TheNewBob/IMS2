#pragma once
/**
 * \brief Event that signifies a change in dockport status
 *
 * Triggered by clbkDockEvent()
 */
class IMS_DockEvent :
	public Event_Base
{
public:
	IMS_DockEvent() : Event_Base(DOCKEVENT, 1){};
	~IMS_DockEvent(){};

private:
};

