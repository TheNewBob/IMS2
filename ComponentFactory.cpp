#include "Common.h"
#include "Components.h"
#include "IMS_ModuleDataManager.h"
#include "ComponentFactory.h"


ComponentFactory::ComponentFactory()
{
}


ComponentFactory::~ComponentFactory()
{
}


IMS_Component_Base *ComponentFactory::CreateNew(string componentname, IMS_Module *containingmodule)
{
	auto data = IMS_ModuleDataManager::GetComponentModel(componentname);

	if (data->GetType() == COMPTYPE_LIGHTING)
		return new IMS_Component_Lighting((IMS_Component_Model_Lighting*)data, containingmodule);

}

IMS_Component_Base *ComponentFactory::CreateFromSerializedString(string serializedstring, IMS_Module *containingmodule)
{
	auto startOfData = serializedstring.find_first_of('{');
	auto endOfData = serializedstring.find_last_of('}');
	auto type = serializedstring.substr(0, startOfData);
	auto data = serializedstring.substr(startOfData, endOfData);

	auto component = CreateNew(type, containingmodule);
	component->Deserialize(data);
	return component;
}
