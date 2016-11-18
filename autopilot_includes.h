/**
 * \file Include this file to use autopilots
 */
#pragma once


/**
* \brief Defines available AP modes.
*/
enum AP_MODE
{
	KILLROT,			//!< Identifies the killrot autopilot in the IMS vessel.
};

#include "IMS_AutopilotOperation_Base.h"
#include "IMS_AutopilotOperation_Killrot.h"
#include "IMS_Autopilot_Base.h"
#include "IMS_Autopilot_Killrot.h"