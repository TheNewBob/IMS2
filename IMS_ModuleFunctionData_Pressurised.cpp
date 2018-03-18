#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"


IMS_ModuleFunctionData_Pressurised::IMS_ModuleFunctionData_Pressurised()
{
	type = MTYPE_PRESSURISED;
	if (name == "") name = "Pressurised compartment";
}


IMS_ModuleFunctionData_Pressurised::~IMS_ModuleFunctionData_Pressurised()
{
}




