#include "Common.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Factory.h"
#include "Oparse.h"

using namespace Oparse;

Oparse::OpModelDef IMS_General_ModuleData::GetModelDef()
{
	return MergeModelDefs(
		IMS_ModuleFunction_Factory::GetModuleFunctionDataFactories(functiondata),
		Oparse::OpModelDef() = {
			{ "mass", { _Param(_mass), { _REQUIRED() } } },
			{ "name", { _Param(_name), {} } }
		});
}


IMS_General_ModuleData::IMS_General_ModuleData()
	: _mass(-1), _name("")
{
}


IMS_General_ModuleData::~IMS_General_ModuleData()
{
}

double IMS_General_ModuleData::getMass()
{
	return _mass;
}


string IMS_General_ModuleData::getName()
{
	return _name;
}

void IMS_General_ModuleData::PostParse()
{
	for (UINT i = 0; i < functiondata.size(); ++i)
	{
		functiondata[i]->PostParse();
	}
}
