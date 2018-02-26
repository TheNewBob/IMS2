#include "GuiIncludes.h"
#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_Location.h"
#include "IMS_Movable.h"
#include "IMS_Component_Model_Base.h"
#include "IMS_Component_Model_Lighting.h"
#include "IMS_Component_Base.h"
#include "IMS_Component_Lighting.h"


IMS_Component_Lighting::IMS_Component_Lighting(IMS_Component_Model_Lighting *data, IMS_Location *location)
	: IMS_Component_Base(data, location)
{
}


IMS_Component_Lighting::~IMS_Component_Lighting()
{
}

string IMS_Component_Lighting::Serialize()
{
	return "lighting";
}

void IMS_Component_Lighting::Deserialize(string data)
{

}

double IMS_Component_Lighting::GetMass()
{
	return data->GetEmptyMass();
}

void IMS_Component_Lighting::PreStep(IMS_Location *location, double simdt)
{

}

bool IMS_Component_Lighting::ProcessEvent(Event_Base *e)
{
	return false;
}

void IMS_Component_Lighting::getDynamicData(map<string, string> &keysAndValues)
{

}

void IMS_Component_Lighting::setDynamicData(map<string, string> &keysAndValues)
{

}

