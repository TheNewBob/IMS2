#include "Common.h"
#include "Events.h"
#include "Moduletypes.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Location.h"
#include "IMS_Movable.h"
#include "IMS_Component_Base.h"
#include "ComponentFactory.h"
#include "IMS.h"
#include "IMS_Module.h"



IMS_ModuleFunction_Location::IMS_ModuleFunction_Location(IMS_ModuleFunctionData_Location *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> contexts, double maxVolume)
	: IMS_ModuleFunction_Base(_data, _module, _type), IMS_Location(contexts), maxVolume(maxVolume), availableVolume(maxVolume), data(_data)
{
}


IMS_ModuleFunction_Location::~IMS_ModuleFunction_Location()
{
}


IMS_Component_Base *IMS_ModuleFunction_Location::CreateComponent(string componentName)
{
	auto newComponent = ComponentFactory::CreateNew(componentName, this);
	if (newComponent != NULL)
	{
		components.push_back(newComponent);
		calculateComponentProperties();
		addEvent(new MassHasChangedEvent(), INSIDE_MODULE_PIPE);
	}
	else
	{
		Olog::error("Component with name %s could not be created!", componentName);
	}
	return newComponent;
}

void IMS_ModuleFunction_Location::RemoveComponent(IMS_Component_Base *component)
{
	auto it = find(components.begin(), components.end(), component);
	Olog::assertThat([&]() { return it != components.end(); }, "Attempting to Remove component from module that was never added!");
	components.erase(it);
	RemoveMovable(component);
	delete component;
	calculateComponentProperties();
	addEvent(new MassHasChangedEvent(), INSIDE_MODULE_PIPE);
}

void IMS_ModuleFunction_Location::calculateComponentProperties()
{
	availableVolume = maxVolume;
	componentMass = 0;
	for (UINT i = 0; i < components.size(); ++i)
	{
		availableVolume -= components[i]->GetVolume();
		componentMass += components[i]->GetMass();
	}
}

IMS_ModuleFunctionData_Location *IMS_ModuleFunction_Location::GetData()
{
	return data;
}

void IMS_ModuleFunction_Location::PreStep(double simdt, IMS2 *vessel)
{
	//todo: movables!
	InvokeMovablePreStep(simdt);
	IMS_ModuleFunction_Base::PreStep(simdt, vessel);
}

void IMS_ModuleFunction_Location::GetAddableComponentModels(vector<IMS_Component_Model_Base*> &OUT_componentModels)
{
	module->GetVessel()->GetAddableComponents(contexts, OUT_componentModels);
}

IMS_Module *IMS_ModuleFunction_Location::GetModule()
{
	return IMS_ModuleFunction_Base::GetModule();
}

void IMS_ModuleFunction_Location::GetInstalledComponents(vector<IMS_Component_Base*> &OUT_components)
{
	OUT_components = components;
}

void IMS_ModuleFunction_Location::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, "\tBEGIN_COMPONENTS");
	for (UINT i = 0; i < components.size(); ++i)
	{
		oapiWriteLine(scn, (char*)string("\t\t" + components[i]->Serialize()).data());
	}
	oapiWriteLine(scn, "\tEND_COMPONENTS");
}

bool IMS_ModuleFunction_Location::processScenarioLine(string line)
{
	static bool readingComponents = false;
	bool lineConsumed = false;
	if (line == "BEGIN_COMPONENTS")
	{
		readingComponents = true;
		lineConsumed = true;
	}
	else if (line == "END_COMPONENTS")
	{
		readingComponents = false;
		lineConsumed = true;
	}
	else if (readingComponents)
	{
		vector<string> tokens;
		Helpers::Tokenize(line, tokens, "{}");
		string componentName = tokens[0];
		Helpers::stringToLower(componentName);
		auto newComponent = CreateComponent(componentName);
		if (newComponent != NULL && tokens.size() > 1)
		{
			newComponent->Deserialize(tokens[1]);
		}
		else if (newComponent == NULL)
		{
			Olog::error("Failed to create component %s from scenario file!", tokens[0]);
		}
		lineConsumed = true;
	}
	return lineConsumed;
}