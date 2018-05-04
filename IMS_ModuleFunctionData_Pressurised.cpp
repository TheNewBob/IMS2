#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "Oparse.h"

using namespace Oparse;

OpModelDef IMS_ModuleFunctionData_Pressurised::GetModelDef()
{
	return OpModelDef() = {
		{ "name", { _Param(name), {} } },
		{ "volume", { _Param(volume), {} } },
	};
}


IMS_ModuleFunctionData_Pressurised::IMS_ModuleFunctionData_Pressurised()
{
	type = MTYPE_PRESSURISED;
	if (name == "") name = "Pressurised compartment";
}


IMS_ModuleFunctionData_Pressurised::~IMS_ModuleFunctionData_Pressurised()
{
}
