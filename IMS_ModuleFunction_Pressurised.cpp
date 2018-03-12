#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_Module.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Location.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_Movable.h"


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Location(_data, _module, MTYPE_PRESSURISED, { IMS_Location::CONTEXT_PRESSURISED }, _data->GetVolume())
{

}


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> locationContexts)
	: IMS_ModuleFunction_Location(_data, _module, _type, locationContexts, _data->GetVolume())
{

}


IMS_ModuleFunction_Pressurised::~IMS_ModuleFunction_Pressurised()
{
}


bool IMS_ModuleFunction_Pressurised::ProcessEvent(Event_Base *e)
{
	return false;
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