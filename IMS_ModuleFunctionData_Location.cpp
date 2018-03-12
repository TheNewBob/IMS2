#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"


IMS_ModuleFunctionData_Location::IMS_ModuleFunctionData_Location()
{
}


IMS_ModuleFunctionData_Location::~IMS_ModuleFunctionData_Location()
{
}

double IMS_ModuleFunctionData_Location::GetVolume()
{
	return volume;
}


bool IMS_ModuleFunctionData_Location::processConfigLine(vector<string> &tokens)
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



bool IMS_ModuleFunctionData_Location::validateData(string configfile)
{
	bool validationok = true;
	if (volume == -1)
	{
		Olog::error("modulefunction MUST have volume declared in %s", configfile.data());
		validationok = false;
	}
	return validationok;
}
