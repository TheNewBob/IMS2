#include "GuiIncludes.h"
#include "Common.h"
#include "Events.h"
#include "IMS_Movable.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_Component_Model_Base.h"
#include "IMS_Component_Model_Base.h"
#include "IMS_Component_Base.h"


IMS_Component_Base::IMS_Component_Base(IMS_Component_Model_Base *data, IMS_Location *location) 
	: IMS_Movable(location), data(data)
{
}


IMS_Component_Base::~IMS_Component_Base()
{
}

string IMS_Component_Base::Serialize()
{
	map<string, string> keysAndValues;
	getDynamicData(keysAndValues);
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
