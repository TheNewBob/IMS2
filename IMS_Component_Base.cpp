#include "GuiIncludes.h"
#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_Component_Model_Base.h"
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
	//TODO: disconnect event handlers? Not actually sure.
}

void IMS_Component_Base::PreStateUpdate()
{
	processWaitingQueue();
}

string IMS_Component_Base::Serialize()
{
	auto keysAndValues = getDynamicData();
	string strData = data->GetType() + "{";
	for (auto i = keysAndValues.begin(); i != keysAndValues.end(); ++i)
	{
		strData += (i->first + ":" + i->second + ";");
	}
	return strData + "}";
}

void IMS_Component_Base::Deserialize(string data)
{
	vector<string> items;
	Helpers::Tokenize(data, items, ";");
	map<string, string> keysAndValues;
	for (auto i = items.begin(); i != items.end(); ++i)
	{
		vector<string> keyValue;
		Helpers::Tokenize((*i), keyValue, ":");
		keysAndValues[keyValue[0]] = keyValue[1];
	}
	setDynamicData(keysAndValues);
}

