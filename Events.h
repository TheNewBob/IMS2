#pragma once

/**
 * \file Events.h
 * Central include header for event classes. Include this header to get access to all event functionality
 */

#include <queue>
#include "EventTypes.h"
#include "Event_Base.h"
#include "Event_Timed.h"
#include "EventNode.h"
#include "EventSink.h"
#include "EventGenerator.h"
#include "EventHandler.h"

//include the different event classes
#include "IMS_DockEvent.h"
#include "ShiftCGEvent.h"
#include "CoGHasChangedEvent.h"
#include "LiftoffEvent.h"
#include "TouchdownEvent.h"
#include "MassHasChangedEvent.h"
#include "VesselVisualCreatedEvent.h"
#include "VesselVisualDestroyedEvent.h"
#include "ConsumableAddedEvent.h"
#include "RcsChangedEvent.h"
#include "SimulationStartedEvent.h"

//animation events
#include "AnimationEvent_Base.h"
#include "StartAnimationEvent.h"
#include "StopAnimationEvent.h"
#include "AnimationStartedEvent.h"
#include "AnimationFailedEvent.h"
#include "AnimationFinishedEvent.h"
#include "StartTrackingAnimationEvent.h"
#include "TrackingAnimationStatusEvent.h"
#include "VesselLayoutChangedEvent.h"
#include "ModifyAnimationEvent.h"
#include "ModifyTrackingAnimationEvent.h"