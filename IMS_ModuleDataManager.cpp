#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "Components.h"
#include "IMS_ModuleDataManager.h"
#include "ComponentFactory.h"
#include <algorithm>
#include "IMS_Location.h"
#include "Oparse.h"

using namespace Oparse;

std::map<string, STATICMODULEDATA> IMS_ModuleDataManager::_staticModuleData;
std::vector<CONSUMABLEDATA> IMS_ModuleDataManager::consumabledata;
std::map<string, IMS_Component_Model_Base*> IMS_ModuleDataManager::components;

const string COMPONENTS_ROOT = "config/IMS2/components/";

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
		auto result = ParseFile(cfg.file, data.orbiterData->GetModelDef(), configfile);
		if (result.HasErrors())
		{
			Olog::error((char*)result.GetFormattedErrorsForFile().data());
			throw runtime_error("Aborting due to critical parsing errors, see log!");
		}
		data.orbiterData->_configFileName = configfile;
		data.orbiterData->PostParse();

		//data.orbiterData->LoadFromFile(configfile, cfg);
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
	Olog::assertThat([]() { return consumabledata.size() > 0; }, "No consumable data defined. Possible missing or corrupt file");

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

IMS_Component_Model_Base *IMS_ModuleDataManager::GetComponentModel(string name)
{
	if (components.size() == 0)
	{
		Olog::debug("Loading component models");
		loadComponentData(COMPONENTS_ROOT);
	}
	transform(name.begin(), name.end(), name.begin(), tolower);
	auto model = components[name];
	if (model == NULL)
	{
		Olog::error("Attempting to load component model of type %s, but no such component model exists!", name.data());
		throw runtime_error("fatal error while loading component");
	}
	return model;
}

vector<IMS_Component_Model_Base*> IMS_ModuleDataManager::GetAllComponentModels()
{
	if (components.size() == 0)
		loadComponentData(COMPONENTS_ROOT);
	
	vector<IMS_Component_Model_Base*> result;
	for (auto it = components.begin(); it != components.end(); ++it)
	{
		result.push_back(it->second);
	}
	return result;
}

vector<IMS_Component_Model_Base*> IMS_ModuleDataManager::GetComponentModelsForContexts(vector<LOCATION_CONTEXT> &IN_contexts)
{
	if (components.size() == 0)
		loadComponentData(COMPONENTS_ROOT);

	vector<IMS_Component_Model_Base*> result;
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		auto componentContexts = i->second->GetContexts();
		for (UINT j = 0; j < IN_contexts.size(); ++j)
		{
			if (find(componentContexts.begin(), componentContexts.end(), IN_contexts[j]) != componentContexts.end())
			{
				result.push_back(i->second);
				break;
			}
		}
	}
	return result;
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
						Olog::error("consumable has no name in Config/IMS2/consumables.cfg");
						valid = false;
					}
					if (consumabledata.back().density == -1)
					{
						if (!valid)					//the gojuice doesn't even have a name
						{
							Olog::error("consumable has no density in Config/IMS2/consumables.cfg");
						}
						else
						{
							Olog::error("consumable has no density in %s  in Config/IMS2/consumables.cfg", consumabledata.back().name.data());
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
								Olog::warn("unknown consumable type: %s in %s in Config/IMS2/consumables.cfg",
									tokens[1].data(), consumabledata.back().name.data());
							}
							else
							{
								Olog::warn("unknown consumable type: %s  in Config/IMS2/consumables.cfg", tokens[1].data());
							}
						}
					}
					else
					{
						Olog::warn("Unknown parameter '%s\' in Config/IMS2/consumables.cfg", tokens[0].data());
					}
				}
			}
			tokens.clear();
		}
		file.close();
	}
	else
	{
		Olog::warn("Could not find file Config/IMS2/consumables.cfg!");
	}
}

void IMS_ModuleDataManager::loadComponentData(string path)
{
	Olog::trace("Loading components from %s", path.data());
	// walk through subfolders and load the stylesets there.
	string searchstr = path + "*.*";
	
	WIN32_FIND_DATA	searchresult;
	HANDLE hfind = NULL;

	hfind = FindFirstFile(searchstr.data(), &searchresult);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		Olog::warn("No files or subfolders found in %s", path.data());
	}
	else do
	{
		if (searchresult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			string subfolder = searchresult.cFileName;
			if (subfolder != "." && subfolder != "..")
			{
				loadComponentData(path + subfolder + "/");
			}
		}
		else 
		{
			string filename = searchresult.cFileName;
			transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			if (filename.length() > 4 && filename.compare(filename.length() - 4, filename.length(), ".cmp") == 0)
			{
				string fullpath = path + filename;
				Olog::trace("Loading component file %s", fullpath.data());
				auto cmp = IMSFILE(oapiOpenFile(fullpath.data(), FILE_IN));
				char typechars[128];
				bool typeDefined = oapiReadItem_string(cmp.file, "Type", typechars);
				string type = typechars;
				transform(type.begin(), type.end(), type.begin(), ::tolower);
				if (typeDefined)
				{
					auto model = ComponentFactory::CreateModel(type);
					if (model != NULL)
					{
						model->LoadFromFile(fullpath.data(), cmp);
						string componentName = model->GetName();
						transform(componentName.begin(), componentName.end(), componentName.begin(), ::tolower);
						auto i = components.find(componentName);
						if (i == components.end())
						{
							if (model->Validate(fullpath))
							{
								components[componentName] = model;
							}
							else
							{
								Olog::error("%s contains errors, component not loaded", fullpath.data());
							}
						}
						else
						{
							Olog::error("Duplicate component name: %s, not loading file %s", model->GetName().data(), fullpath.data());
							delete model;
						}
					}
					else
					{
						Olog::error("Unknown type '%s' in component %s, not loading file", type.data(), fullpath.data());
					}
				}
				else
				{
					Olog::error("Error while instantiating component from file %s", fullpath.data());
				}
			}
			else
			{
				Olog::warn("File %s%s is not a component file!", path.data(), filename.data());
			}
		}
	} while (FindNextFile(hfind, &searchresult));
	FindClose(hfind);
}


void IMS_ModuleDataManager::Cleanup()
{
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		delete i->second;
	}
	components.clear();
}
