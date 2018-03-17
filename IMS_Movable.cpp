#include "GUIincludes.h"
#include "Common.h"
#include "ModuleTypes.h"
#include "Events.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_Movable.h"
#include "IMS_Location.h"


IMS_Movable::IMS_Movable(IMS_Location *location, bool fixed) : EventHandler(), fixedpos(fixed)
{
	//initialise event handler
	eventhandlertype = MOVABLE_HANDLER;
	//event generator that sends events to module
	createEventGenerator(INSIDE_MODULE_PIPE);
	//event sink that receives events from module
	createEventSink(INSIDE_MODULE_PIPE);

	location->AddMovable(this);
}


IMS_Movable::~IMS_Movable()
{
}


void IMS_Movable::Move(IMS_Location *from, IMS_Location *to)
{
	if (!fixedpos)
	{
		Olog::assertThat([from, to]() { return from != to; }, "passed the same module for to and from");
		from->RemoveMovable(this);
		to->AddMovable(this);
	}
}


void IMS_Movable::removeFrom(IMS_Location *location)
{
//	location->RemoveMovable(this);
	//disconnect event handler
	disconnect(location->GetModule());
}

void IMS_Movable::addTo(IMS_Location *location)
{
//	location->AddMovable(this);
	//connect the modules' EventSink for module internal events to my generator
	location->GetModule()->ConnectMySinkToYourNode(getEventGenerator(INSIDE_MODULE_PIPE), INSIDE_MODULE_PIPE);
	//connect my eventsink for module internal events to the module
	location->GetModule()->ConnectToMyEventSink(getEventSink(INSIDE_MODULE_PIPE), INSIDE_MODULE_PIPE);
}

void IMS_Movable::PreStateUpdate()
{
	processWaitingQueue();
}
