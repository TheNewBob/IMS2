#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "SimpleShape.h"
#include "IMS_General_ModuleData.h"
#include "Managers.h"
#include "IMS_CoGmanager.h"
#include "IMS_TouchdownPointManager.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_ModuleDataManager.h"


SPLIT_VESSEL_DATA IMS2::SplitVesselData;

void IMS2::clbkSetClassCaps (FILEHANDLE cfg) 
{
	//create the vessel from file with a new module
	if (!IMS2::SplitVesselData.split)
	{
		//since orbiter doesn't give us the name of the file, we have to reconstruct it
		stringstream filename("");
		filename << GetClassNameA() << ".cfg";

		//load the config
		STATICMODULEDATA data = IMS_ModuleDataManager::GetStaticModuleData(filename.str(), IMSFILE(cfg));
		AddNewModule(data.orbiterData, data.moduleData);
	}
	//create the vessel from transfer data of a split vessel with existing modules
	else
	{
		for (UINT i = 0; i < IMS2::SplitVesselData.modules.size(); ++i)
		{
			modules.push_back(IMS2::SplitVesselData.modules[i]);
		}
	}
}


void IMS2::clbkSetStateEx(const void *status)
{
	//this vessel was either created by a split, or by config on runtime.

	VESSEL2::clbkSetStateEx(status);

	postLoad(IMS2::SplitVesselData.split);

	//if created by split, we also have to rotate the vessel into proper alignement, clean up the mess, and initiate CoG-shift
	if (IMS2::SplitVesselData.split)
	{
		SetRotationMatrix(IMS2::SplitVesselData.transform);
		IMS2::SplitVesselData.clear();
		addEvent(new ShiftCGEvent(1));
	}

	//create docking ports now
	UpdateDockPorts(true);
}



void IMS2::postLoad(bool created_from_split)
{
	//do postload for modules if the vessel is created from file
	if (!created_from_split)
	{
		for (UINT i = 0; i < modules.size(); ++i)
		{
			modules[i]->PostLoad();
		}
	}

	//add all modules to the vessel	
	for (UINT i = 0; i < modules.size(); ++i)
	{
		if (i == 0)
		{
			modules[i]->AddModuleToVessel(this, false, false);
		}
		else
		{
			modules[i]->AddModuleToVessel(this);
		}

		//if the vessel was created by split, we also have to reinitialise all the attachment points
		if (created_from_split)
		{
			modules[i]->CreatePhysicalAttachmentPoints();
			modules[i]->TransformAttachmentPoints();
		}
	}

	//call postload on the managers
	for (auto i = managers.begin(); i != managers.end(); ++i)
	{
		i->second->PostLoad();
	}
}


void IMS2::clbkPostCreation() 
{
	//initialise tracking of the vessel state
	updateVesselState(false);
}


void IMS2::clbkSaveState (FILEHANDLE scn)
{
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->SaveState(scn, i);
	}

	stringstream ss;
	//write the current CoG offset, since we'll have to set it before the vessel will be fully initialised on load
	VECTOR3 cogpos = GetCoGmanager()->GetCoG();
	oapiWriteScenario_vec(scn, "COG", cogpos);
	//write the default orbiter data
	VESSEL3::clbkSaveState(scn);
}




void IMS2::clbkLoadStateEx(FILEHANDLE scn, void *status)
{
	bool firstModuleProcessed = false;			//needed to skip loading of first module, which was already initialised in SetClassCaps

	char *line;
	while (oapiReadScenario_nextline(scn, line))
	{
		string s = line;

		if (s.compare(0, 12, "MODULE_BEGIN") == 0)
		{
			//load a new module
			if (!LoadModuleFromScenario(scn, firstModuleProcessed))
			{
				return;
			}
			firstModuleProcessed = true;
		}
		else if (s.compare(0, 3, "COG") == 0)
		{
			//load and initialise the center of gravity
			VECTOR3 cogpos;
			sscanf(s.substr(4).data(), "%lf %lf %lf",
				&cogpos.x, &cogpos.y, &cogpos.z);
			GetCoGmanager()->InitialiseCoG(cogpos);
		}
		else if (s.compare(0, 8, "PRPLEVEL") == 0)
		{
			//We don't need the propellant levels.
			//we don't NEED the propellant levels.
			//we manage our propellant levels ourselves, thank you very much!
		}
		else if (s.compare(0, 7, "THLEVEL") == 0)
		{
			//We don't need thrust levels either. It's really not much use in any case.
			//unntil the day it was making me trouble I didn't even realise thruster levels got saved.
		}
		else
		{
			ParseScenarioLineEx(line, status);
		}
	}

	postLoad(false);
	//create the docking ports so orbiter can dock the vessels in the next step
	UpdateDockPorts();

}

bool IMS2::LoadModuleFromScenario(FILEHANDLE scn, bool initFromCfg)
//allocates, initialises and loads module from scenario, returns true if successful
{
	char *line;
	oapiReadScenario_nextline (scn, line);
	string s = line;

	if (s.compare(0, 4, "FILE") == 0 && initFromCfg)	
	//this step is not necessary for the first module, it is already initialised in clbkSetClassCaps
	{
		//check if the module config was already loaded
		STATICMODULEDATA data = IMS_ModuleDataManager::GetStaticModuleData(s.substr(5), IMSFILE(NULL));
		if (data.moduleData != NULL && data.orbiterData != NULL)
		{
			if (!AddNewModule(data.orbiterData, data.moduleData))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	//load the state of the new module from scenario
	modules[modules.size() - 1]->LoadState(scn);
			
	return true;
}

bool IMS2::AddNewModule(IMS_Orbiter_ModuleData *orbiter_data, IMS_General_ModuleData *module_data)
{
	IMS_Module *newModule = new IMS_Module();


	newModule->InitModule(orbiter_data, module_data, this);
	modules.push_back(newModule);
	return true;
}


