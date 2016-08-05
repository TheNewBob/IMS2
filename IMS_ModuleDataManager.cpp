#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleDataManager.h"

std::map<string, STATICMODULEDATA> IMS_ModuleDataManager::_staticModuleData;
std::vector<CONSUMABLEDATA> IMS_ModuleDataManager::consumabledata;


IMS_ModuleDataManager::IMS_ModuleDataManager()
{
}


IMS_ModuleDataManager::~IMS_ModuleDataManager()
{
}


STATICMODULEDATA IMS_ModuleDataManager::GetStaticModuleData(string configfile, IMSFILE cfg)
//returns a pointer to the IMS_General_ModuleData with the passed name if it was already loaded, NULL otherwise
{
//	GetConsumableData("dummy");

	//get the data from the map
	STATICMODULEDATA data = _staticModuleData[configfile];
	//if not loaded yet, load the config file. Technically, they should both be either NULL or valid
	if (data.moduleData == NULL || data.orbiterData == NULL)
	{
		if (cfg.file == NULL)
		//open the file if it isn't open already
		{
			string fullpath = "Vessels\\" + configfile;
			cfg.file = oapiOpenFile(fullpath.c_str(), FILE_IN, CONFIG);
		}

		//get the module type from the config
		data.moduleData = new IMS_General_ModuleData;
		data.orbiterData = new IMS_Orbiter_ModuleData;

		//load the data from the config file and put them in the map
		data.orbiterData->LoadFromFile(configfile, cfg);
		data.moduleData->LoadFromFile(configfile, cfg);

		_staticModuleData[configfile].moduleData = data.moduleData;
		_staticModuleData[configfile].orbiterData = data.orbiterData;
	}
	return data;
}


CONSUMABLEDATA *IMS_ModuleDataManager::GetConsumableData(string gojuicetype)
{
	//if the data wasn't loaded from file yet, load it now!
	if (consumabledata.size() == 0)
	{
		loadConsumableData();
	}

	vector<CONSUMABLEDATA>::iterator i = find(consumabledata.begin(), consumabledata.end(), gojuicetype);
	if (i == consumabledata.end()) return NULL;
	
	return &(*i);						//C++ synthax can be horrible at times...
}


int IMS_ModuleDataManager::GetConsumableId(string consumableshort)
{
	//if the data wasn't loaded from file yet, load it now!
	if (consumabledata.size() == 0)
	{
		loadConsumableData();
	}

	vector<CONSUMABLEDATA>::iterator i = find(consumabledata.begin(), consumabledata.end(), consumableshort);
	if (i == consumabledata.end()) return -1;

	return (*i).id;						//C++ synthax can be horrible at times...
}


CONSUMABLEDATA *IMS_ModuleDataManager::GetConsumableData(int id)
{
	//if this assert is triggered, something went wrong.
	//GetConsumableData(string) is always called before this if the 
	//program flow is correct. Either it didn't get called first,
	//or it didn't find the file, or the file was empty (i.e. corrupted)
	assert(consumabledata.size() > 0);

	return &consumabledata[id];
}

vector<CONSUMABLEDATA*> IMS_ModuleDataManager::GetCompatibleConsumables(int reference_id)
{
	vector<CONSUMABLEDATA*> compatiblelist;
	CONSUMABLESTATE referencestate = GetConsumableData(reference_id)->state;

	for (UINT i = 0; i < consumabledata.size(); ++i)
	{
		if (consumabledata[i].state == referencestate)
		{
			compatiblelist.push_back(&consumabledata[i]);
		}
	}
	return compatiblelist;
}

void IMS_ModuleDataManager::loadConsumableData()
{
	ifstream file;
	file.open("./Config/IMS2/consumables.cfg");

	if (file)
	{
		string line;
		vector<string> tokens;
		bool readdata = false;

		while (!file.eof())
		{
			getline(file, line);
			Helpers::Tokenize(line, tokens, "=");

			if (tokens.size() > 0)
			{
				//remove tleading and trailing whitespace
				for (UINT i = 0; i < tokens.size(); ++i)
				{
					Helpers::removeExtraSpaces(tokens[i]);
				}
				//ignore case on parameter
				Helpers::stringToLower(tokens[0]);

				if (tokens[0] == "begin_consumable")
				{
					consumabledata.push_back(CONSUMABLEDATA());
					//initialise the data
					consumabledata.back().state = LIQUID;
					consumabledata.back().density = -1;
					consumabledata.back().id = consumabledata.size() - 1;
					readdata = true;
				}

				else if (tokens[0] == "end_consumable" && readdata)
				{
					readdata = false;
					//validate the data
					bool valid = true;
					if (consumabledata.back().name == "")
					{
						Helpers::writeToLog(string("consumable has no name in Config/IMS2/consumables.cfg"), L_ERROR);
						valid = false;
					}
					if (consumabledata.back().density == -1)
					{
						if (!valid)					//the gojuice doesn't even have a name
						{
							Helpers::writeToLog(string("consumable has no density in Config/IMS2/consumables.cfg"), L_ERROR);
						}
						else
						{
							Helpers::writeToLog(string("consumable has no density in " + consumabledata.back().name + " in Config/IMS2/consumables.cfg"), L_ERROR);
						}
					}

				}

				else if (readdata && tokens.size() > 1)
				{
					if (tokens[0] == "name")
					{
						consumabledata.back().name = tokens[1];
					}

					else if (tokens[0] == "shorthand")
					{
						consumabledata.back().shorthand = tokens[1];
					}

					else if (tokens[0] == "density")
					{
						consumabledata.back().density = (float)Helpers::stringToDouble(tokens[1]);
					}

					else if (tokens[0] == "state")
					{
						Helpers::stringToLower(tokens[1]);
						if (tokens[1] == "liquid")
						{ 
							consumabledata.back().state = LIQUID;
						}
						else if (tokens[1] == "solid")
						{
							consumabledata.back().state = SOLID;
						}
						else
						{
							if (consumabledata.back().name != "")
							{
								Helpers::writeToLog(string("unknown consumable type: " + tokens[1] + " in " +
									consumabledata.back().name + " in Config/IMS2/consumables.cfg"), L_WARNING);
							}
							else
							{
								Helpers::writeToLog(string("unknown consumable type: " + tokens[1] + " in Config/IMS2/consumables.cfg"), L_WARNING);
							}
						}
					}
					else
					{
						Helpers::writeToLog(string("Unknown parameter \"" + tokens[0] + "\" in Config/IMS2/consumables.cfg"), L_WARNING);
					}
				}
			}
			tokens.clear();
		}
		file.close();
	}
	else
	{
		Helpers::writeToLog(string("Could not find file Config/IMS2/consumables.cfg!"), L_WARNING);
	}
}
