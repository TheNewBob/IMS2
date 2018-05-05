#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Gear.h"
#include "Oparse.h"

using namespace Oparse;

OpModelDef IMS_ModuleFunctionData_Gear::GetModelDef()
{
	return OpModelDef() = {
		{ "name", { _Param(name), {} } },
		{ "deploy_anim", { _Param(deployanimname), {} } },
		{ "tdpos", { _Param(touchdownpoint), { _REQUIRED() } } },
		{ "tddir", { _Param(tddir), {} } },
		{ "tdstiffness", { _Param(tdstiffness), {} } },
		{ "tddamping", { _Param(tddamping), {} } }
	};
}

void IMS_ModuleFunctionData_Gear::PostParse()
{
	if (deployanimname != "") candeploy = true;
}


IMS_ModuleFunctionData_Gear::IMS_ModuleFunctionData_Gear()
{
	type = MTYPE_GEAR;
}


IMS_ModuleFunctionData_Gear::~IMS_ModuleFunctionData_Gear()
{
}
