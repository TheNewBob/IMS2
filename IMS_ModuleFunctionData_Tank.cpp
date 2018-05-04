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


bool IMS_ModuleFunctionData_Tank::processConfigLine(vector<string> &tokens)
{
	//the volume is loaded by the base class
	if (!IMS_ModuleFunctionData_Pressurised::processConfigLine(tokens))
	{
		if (tokens[0] == "contains")
		{
			propellant = IMS_ModuleDataManager::GetConsumableData(tokens[1]);
			return true;
		}
		else if (tokens[0] == "convertible")
		{
			if (tokens[1] == "false")
			{
				convertible = false;
				return true;
			}
		}
	}
	else
	{
		//parameter has been read by the base class
		return true;
	}
	return false;
}



bool IMS_ModuleFunctionData_Tank::validateData(string configfile)
{
	if (IMS_ModuleFunctionData_Pressurised::validateData(configfile))
	{
		if (propellant != NULL)
		{
			//check if the tank is convertable
			if (convertible)
			{
				//tanks with solid fuels or less than 3 m^3 volume cannot be converted
				if (propellant->state == SOLID || volume < 3)
				{
					convertible = false;
				}
			}
			return true;
		}
		else
		{
			Olog::error("No valid consumable declared as propellant in %s!", configfile.data());
		}
	}
	//if either of the above conditions doesn't check out, the tank is unusable
	return false;
}