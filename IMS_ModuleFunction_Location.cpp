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


IMS_ModuleFunction_Location::IMS_ModuleFunction_Location(IMS_ModuleFunctionData_Location *_data, IMS_Module *_module, FUNCTIONTYPE _type, vector<LOCATION_CONTEXT> contexts, double maxVolume)
	: IMS_ModuleFunction_Base(_data, _module, _type), IMS_Location(contexts), maxVolume(maxVolume), data(_data)
{
}


IMS_ModuleFunction_Location::~IMS_ModuleFunction_Location()
{
}


void IMS_ModuleFunction_Location::CreateComponent(string componentName)
{
	auto newComponent = ComponentFactory::CreateNew(componentName, this);
	AddMovable(newComponent);
	components.push_back(newComponent);
	calculateComponentProperties();
}

void IMS_ModuleFunction_Location::RemoveComponent(IMS_Component_Base *component)
{
	auto it = find(components.begin(), components.end(), component);
	Olog::assertThat([&]() { return it != components.end(); }, "Attempting to Remove component from module that was never added!");
	components.erase(it);
	RemoveMovable((*it));
	delete (*it);
	calculateComponentProperties();
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

double IMS_ModuleFunction_Location::GetComponentMass()
{

}