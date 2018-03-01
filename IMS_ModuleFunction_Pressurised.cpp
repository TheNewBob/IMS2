#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_Module.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_Movable.h"


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Base(_data, _module, MTYPE_PRESSURISED), data(_data), volume(_data->getVolume()),
	IMS_Location({IMS_Location::CONTEXT_PRESSURISED})
{

}


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> locationContexts)
	: IMS_ModuleFunction_Base(_data, _module, _type), data(_data), volume(_data->getVolume()),
	IMS_Location(locationContexts)
{

}


IMS_ModuleFunction_Pressurised::~IMS_ModuleFunction_Pressurised()
{
}


bool IMS_ModuleFunction_Pressurised::ProcessEvent(Event_Base *e)
{
	return false;
}

void IMS_ModuleFunction_Pressurised::PreStep(double simdt, IMS2 *vessel)
{
	//todo: movables!
	InvokeMovablePreStep(simdt);
	IMS_ModuleFunction_Base::PreStep(simdt, vessel);
}



double IMS_ModuleFunction_Pressurised::GetMass()
{
	double currentmass = GetMovableMass();
	
	return currentmass;
}

IMS_Module *IMS_ModuleFunction_Pressurised::GetModule()
{
	return module;
}