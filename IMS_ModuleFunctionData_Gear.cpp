#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Gear.h"


IMS_ModuleFunctionData_Gear::IMS_ModuleFunctionData_Gear()
{
	type = MTYPE_GEAR;
}


IMS_ModuleFunctionData_Gear::~IMS_ModuleFunctionData_Gear()
{
}


bool IMS_ModuleFunctionData_Gear::processConfigLine(vector<string> &tokens)
{
	//check if line is a valid key - value pair
	if (tokens.size() == 2)
	{
		if (tokens[0].compare("deploy_anim") == 0)
		{
			deployanimname = tokens[1];
			return true;
		}
		else if (tokens[0].compare("tdpos") == 0)
		{
			if (tokens.size() != 4)
			{
				throw runtime_error("Illegal definition of tdpos: parameter must have 3 values (x y z)!");
			}
			touchdownpoint = _V(Helpers::stringToDouble(tokens[1]),
				Helpers::stringToDouble(tokens[2]),
				Helpers::stringToDouble(tokens[3]));
			return true;
		}
		else if (tokens[0].compare("tddir") == 0)
		{
			if (tokens.size() != 4)
			{
				throw runtime_error("Illegal definition of tddir: parameter must have 3 values (x y z)!");
			}
			tddir = _V(Helpers::stringToDouble(tokens[1]),
				Helpers::stringToDouble(tokens[2]),
				Helpers::stringToDouble(tokens[3]));
			return true;
		}
	}
	else
	{
		throw runtime_error("WARNING: Invalid line: " + Helpers::WriteTokenizedString(tokens) + ". Expected <Parameter> = <Value>");
	}
	return false;
}


bool IMS_ModuleFunctionData_Gear::validateData(string configfile)
{
	bool validationok = true;
	if (deployanimname != "")
	{
		//if no deploy animation has been declared, it simply means that this gear is always out.
		candeploy = true;
	}
	
	if (touchdownpoint.x == NAN || touchdownpoint.y == NAN || touchdownpoint.z == NAN)
	{
		Helpers::writeToLog("Modulefunction GEAR  MUST have tdpos defined in " + configfile, L_ERROR);
		validationok = false;
	}
	return validationok;
}

