#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"


IMS_ModuleFunctionData_Pressurised::IMS_ModuleFunctionData_Pressurised()
	: volume(-1)
{
	type = MTYPE_PRESSURISED;
}


IMS_ModuleFunctionData_Pressurised::~IMS_ModuleFunctionData_Pressurised()
{
}

double IMS_ModuleFunctionData_Pressurised::getVolume()
{
	return volume;
}


bool IMS_ModuleFunctionData_Pressurised::processConfigLine(vector<string> &tokens)
{
	//check if line is a valid key - value pair
	if (tokens.size() == 2)
	{
		if (tokens[0].compare("volume") == 0)
		{
			volume = Helpers::stringToDouble(tokens[1]);
			return true;
		}
	}
	else
	{
		throw runtime_error("WARNING: Invalid line: " + Helpers::WriteTokenizedString(tokens) + ". Expected <Parameter> = <Value>");
	}
	return false;
}



bool IMS_ModuleFunctionData_Pressurised::validateData(string configfile)
{
	bool validationok = true;
	if (volume == -1)
	{
		Helpers::writeToLog("pressurised module MUST have volume declared in " + configfile, L_ERROR);
		validationok = false;
	}
	return validationok;
}


