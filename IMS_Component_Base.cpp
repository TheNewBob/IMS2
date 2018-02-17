#include "GuiIncludes.h"
#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_Component_Model_Base.h"
#include "IMS_Component_Base.h"


IMS_Component_Base::IMS_Component_Base(IMS_Component_Model_Base *data, IMS_Module *module) 
	: data(data)
{
	//initialise event handler
	eventhandlertype = MODULE_FUNCTION_HANDLER;
	//event generator that sends events to module
	createEventGenerator(INSIDE_MODULE_PIPE);
	//event sink that receives events from module (actually from other ModuleFunctions)
	createEventSink(INSIDE_MODULE_PIPE);
	//event sink that receives events from the vessel (via the module)
	createEventSink(VESSEL_TO_MODULE_PIPE);

	//connect the eventhandlers
	//connect the modules' EventSink for module internal events to my generator
	module->ConnectMySinkToYourNode(getEventGenerator(INSIDE_MODULE_PIPE), INSIDE_MODULE_PIPE);
	//connect the module functions' eventsink for module internal events to my sink
	module->ConnectToMyEventSink(getEventSink(INSIDE_MODULE_PIPE), INSIDE_MODULE_PIPE);
	//connect the module functions' eventsink for vessel events to the modules eventsink for forwarding
	module->ConnectToMyEventSink(getEventSink(VESSEL_TO_MODULE_PIPE), VESSEL_TO_MODULE_PIPE);
}


IMS_Component_Base::~IMS_Component_Base()
{
}


void IMS_Component_Base::LoadState(FILEHANDLE scn)
{
	char *line;
	//read line by line
	while (oapiReadScenario_nextline(scn, line) && _strnicmp(line, "END_COMPONENT", 13))
	{
		processScenarioLine(string(line));
	}
}

void IMS_Component_Base::PreStateUpdate()
{
	processWaitingQueue();
}