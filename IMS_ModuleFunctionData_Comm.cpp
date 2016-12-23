#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Comm.h"


IMS_ModuleFunctionData_Comm::IMS_ModuleFunctionData_Comm()
{
	type = MTYPE_COMM;
}


IMS_ModuleFunctionData_Comm::~IMS_ModuleFunctionData_Comm()
{
}


bool IMS_ModuleFunctionData_Comm::processConfigLine(vector<string> &tokens)
{
	if (tokens[0].compare("tracking_anim") == 0)
	{
		trackinganimname = tokens[1];
		return true;
	}
	else if (tokens[0].compare("deploy_anim") == 0)
	{
		deployanimname = tokens[1];
		return true;
	}
	else if (tokens[0].compare("search_anim") == 0)
	{
		searchanimname = tokens[1];
		return true;
	}

	return false;
}

bool IMS_ModuleFunctionData_Comm::validateData(string configfile)
{
	//there must be at least one set of controls defined, otherwise the whole modulefunction makes no sense!
	if (deployanimname == "" && searchanimname == "" && trackinganimname == "")
	{
		Helpers::writeToLog(string(configfile + ": must have at least one set of controls!"), L_ERROR);
		return false;
	}
	return true;
}

