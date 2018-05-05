#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Comm.h"
#include "Oparse.h"

using namespace Oparse;


OpModelDef IMS_ModuleFunctionData_Comm::GetModelDef()
{
	return Oparse::OpModelDef() = {
		{ "name", { _Param(name), {} } },
		{ "tracking_anim", { _Param(trackinganimname), {} } },
		{ "search_anim", { _Param(searchanimname), {} } },
		{ "deploy_anim", { _Param(deployanimname), {} } }
	};
}


IMS_ModuleFunctionData_Comm::IMS_ModuleFunctionData_Comm()
{
	type = MTYPE_COMM;
}


IMS_ModuleFunctionData_Comm::~IMS_ModuleFunctionData_Comm()
{
}
