#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_ModuleFunctionData_Tank.h"
#include "Oparse.h"

using namespace Oparse;

Oparse::OpModelDef IMS_ModuleFunctionData_Tank::GetModelDef()
{
	return Oparse::OpModelDef() = {
		{ "volume", { _Param(volume), { _REQUIRED() } } },
		{ "contains", {_Param([&](string value) 
			{
				propellant = IMS_ModuleDataManager::GetConsumableData(value);
				if (propellant == NULL) return value + " is not a valid consumable id!";
				else return string("");
			}), {} } }
	};
}


IMS_ModuleFunctionData_Tank::IMS_ModuleFunctionData_Tank()
{
	type = MTYPE_TANK;
}


IMS_ModuleFunctionData_Tank::~IMS_ModuleFunctionData_Tank()
{
}
