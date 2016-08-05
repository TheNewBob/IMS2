#pragma once

/**
 * \brief Notifies that the rcs layout has been changed.
 *
 * Currently only used internally by the RcsManager.
 */
class RcsChangedEvent :
	public Event_Base
{
public:
	/**
	* \param delay you can specify a frame delay for this event, though usually it'll be 0.
	*/
	RcsChangedEvent() : Event_Base(RCSCHANGEDEVENT, 1){};
	~RcsChangedEvent(){};
};
