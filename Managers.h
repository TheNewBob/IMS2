#pragma once
#include "Events.h"
#include "IMS_Manager_Base.h"


/**
 * \brief IDs of the various managers of a vessel. 
 * Extend this when you add new managers.
 */
enum IMS_MANAGER
{
	COG_MANAGER,			//!< center of gravity and vessel mass manager
	PROP_MANAGER,			//!< Propulsion manager
	RCS_MANAGER,			//!< rcs thruster manager
	TDPOINT_MANAGER			//!< Touchdown Point Manager
};