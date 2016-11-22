#include "Common.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Factory.h"



IMS_General_ModuleData::IMS_General_ModuleData()
	: _mass(-1), _name("")
{
}


IMS_General_ModuleData::~IMS_General_ModuleData()
{
}


bool IMS_General_ModuleData::LoadFromFile(string configfile, IMSFILE cfg)
{
	//reset the file to the start
	Helpers::resetFile(cfg);

	//read through the whole file
	vector<string> tokens;
	bool startAttachments = false;
	//split the line by whitespaces and equal sign
	while (Helpers::readLine(cfg, tokens, " \t="))
	{
		//check if line is valid
		if (tokens.size() == 0) continue;
		//put identifier in lowercase
		transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

		if (tokens[0].compare("mass") == 0)
		{
			_mass = Helpers::stringToDouble(tokens[1]);
		}
		else if (tokens[0].compare("name") == 0)
		{
			_name = tokens[1];
		}
		else if (tokens[0].compare("begin_modulefunction") == 0)
		{
			//the config file declared a module function. Create it and let it parse its parameters
			IMS_ModuleFunctionData_Base *data = IMS_ModuleFunction_Factory::CreateNewModuleFunctionData(
														IMS_ModuleFunction_Factory::GetFunctionTypeFromString(tokens[1]));
			if (data->LoadFromFile(configfile, cfg))
			{
				functiondata.push_back(data);
			}
		}
		tokens.clear();
	}
	
	//check if the module has at least one valid function definition
	if (functiondata.size() == 0)
	{
		Helpers::writeToLog(configfile + ": No valid module function defined!", L_WARNING);
		return false;
	}
	return true;
}

double IMS_General_ModuleData::getMass()
{
	return _mass;
}


string IMS_General_ModuleData::getName()
{
	return _name;
}
