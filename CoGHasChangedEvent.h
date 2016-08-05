#pragma once
/**
 * \brief This event can be used to notify modules and module functions that the center of gravity on the vessel has been changed.
 *
 * It is not to be confused with the ShiftCG Event, which serves to inform the vessel that the CoG SHOULD be changed.
 * This event will be fired afterwards and contains information as to where the CoG shifted to.
 * \see ShiftCGEvent
 * \see EventHandler.h
 */
class CoGHasChangedEvent :
	public Event_Base
{
public:
	/**
	 * \param previousCoG The previous position of the center of gravity relative to the vessels core module
	 * \param newCoG The new position of the center of gravity relative to the vessels core module
	 */
	CoGHasChangedEvent(VECTOR3 previousCoG, VECTOR3 newCoG) : Event_Base(CGHASCHANGEDEVENT, 1),
												previousCoGpos(previousCoG), newCoGpos(newCoG){};
	~CoGHasChangedEvent(){};

	/**
	 * \return The position of the CoG before the shift
	*/
	VECTOR3 GetPreviousCoG() { return previousCoGpos; };
	
	/**
	 * \return The position of the current CoG
	*/
	VECTOR3 GetNewCoG() { return newCoGpos; };
	
	/**
	 * \return How much the CoG has moved
	 */
	VECTOR3 GetCoGShift() { return newCoGpos - previousCoGpos; };

private:
	VECTOR3 previousCoGpos;					//!< Stores the CoG before the shift
	VECTOR3 newCoGpos;						//!< Stores the CoG after the shift
	VECTOR3 relativeCoGshift;				//!< Stores the difference between old and new CoG
};

