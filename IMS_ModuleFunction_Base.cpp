
#include "GuiIncludes.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "GUI_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_Module.h"


IMS_ModuleFunction_Base::IMS_ModuleFunction_Base(IMS_ModuleFunctionData_Base *_data, IMS_Module *_module, FUNCTIONTYPE _type)
	: data(_data), module(_module), type(_type)
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


IMS_ModuleFunction_Base::~IMS_ModuleFunction_Base()
{
}


void IMS_ModuleFunction_Base::PreStep(double simdt, IMS2 *vessel)
{
	sendEvents();
}

void IMS_ModuleFunction_Base::PreStateUpdate()
{
	processWaitingQueue();
}

void IMS_ModuleFunction_Base::LoadState(FILEHANDLE scn)
{
	char *line;
	int lines = 0;
	bool readblock = true;
	//read line by line
	while (oapiReadScenario_nextline(scn, line) && _strnicmp(line, "END_MODULEFUNC", 14))
	{
		processScenarioLine(string(line));
	}
}