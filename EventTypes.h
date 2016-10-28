#pragma once

/**
 * \file EventTypes.h
 * Defines identifiers for event types, event pipes EventHaler types
 */

/**
 * \brief Used to quickly identify events without having to cast them
 * \note When writing a new event class, add a new identifer for it here.
 */
typedef enum
{
	DOCKEVENT,
	SHIFTCGEVENT,
	CGHASCHANGEDEVENT,
	VESSELLAYOUTCHANGEDEVENT,
	LIFTOFFEVENT,
	TOUCHDOWNEVENT,
	MASSHASCHANGEDEVENT,
	CONSUMABLEADDEDEVENT,
	RCSCHANGEDEVENT,
	SIMULATIONSTARTEDEVENT,
	HULLSHAPEREMOVEDEVENT,
	TDPOINTSCHANGEDEVENT,
	TOGGLEGEAREVENT,

	STARTANIMATIONEVENT,
	STOPANIMATIONEVENT,
	ANIMATIONFINISHEDEVENT,
	VESSELVISUALCREATEDEVENT,
	VESSELVISUALDESTROYEDEVENT,
	TRACKINGANIMATIONSTATUSEVENT,
	MODIFYANIMATIONEVENT,
	ANIMATIONSTARTEDEVENT,
	ANIMATIONFAILEDEVENT
} EVENTTYPE;


/**
 * \brief Defines the event pipes existing in IMS2
 */
typedef enum
{
	WAITING_PIPE,					//!< The waiting pipe is a pipe that is treated special by event handlers. This pipe will not be processed on sendEvents(), but on processWaitingQueue() instead
	DEFAULT_PIPE,					//!< The default pipe depends on the settings of the respective EventHandler generating the event
	VESSEL_TO_MODULE_PIPE,			//!< Carries events from the vessel to modules, and from there to module functions
	MODULE_TO_VESSEL_PIPE,			//!< Carries events from a module to the vessel
	INSIDE_MODULE_PIPE,				//!< Carries events from a modulefunction to its module and from there to all modulefunctions in the same module
	VESSEL_TO_GUI_PIPE,				//!< Allows the vessel to send events to its GUI surfaces so displays can react to other input than merely from the user
	MANAGER_TO_VESSEL_PIPE			//!< Allows managers to send events to the vessel
} EVENTPIPE;


/**
 * \brief Defines the possible EventHandler types
 * \note EventHandler types in IMS2 signify the kind of object hiding behind the EventHandler
 */
typedef enum
{
	GUISURFACE_HANDLER,
	VESSEL_HANDLER,					
	MODULE_HANDLER,
	MODULE_FUNCTION_HANDLER,
	MOVABLE_HANDLER,
	MANAGER_HANDLER
} EVENTHANDLERTYPE;