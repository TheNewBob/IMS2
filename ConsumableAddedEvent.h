#pragma once

/**
 * \brief Event signifying that content has been added to a storable.
 * \note The caller of this event is expected to be castable to IMS_Storable*,
 * so don't spawn it from classes not derived from it!
 */
class ConsumableAddedEvent :
	public Event_Base
{
public:
	/**
	 * \param previousmass The mass of consumables the storable had before the new content was added
	 */
	ConsumableAddedEvent(double previousmass) : Event_Base(CONSUMABLEADDEDEVENT, 1), previousmass(previousmass){};
	~ConsumableAddedEvent(){};

	/**
	 * \return The mass the caller had before the new content was added
	 */
	double GetPreviousMass() { return previousmass; };

	/**
	 * \brief These events are considered an equal event if they are sent by the same caller.
	 */
	virtual bool operator==(Event_Base *e)
	{
		if (*e == CONSUMABLEADDEDEVENT)
		{
			if (e->GetCaller() == caller) return true;
		}
		return false;
	}

private:
	double previousmass;
};