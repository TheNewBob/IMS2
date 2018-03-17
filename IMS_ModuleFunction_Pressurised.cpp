#include "GUI_Common.h"
#include "Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_Module.h"
#include "IMS_Location.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Location.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "IMS_Movable.h"
#include "GUI_ModuleFunction_Base.h"
#include "GUI_ModuleFunction_Pressurised.h"


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module)
	: IMS_ModuleFunction_Location(_data, _module, MTYPE_PRESSURISED, { IMS_Location::CONTEXT_PRESSURISED }, _data->GetVolume()), data(_data)
{
	menu = new GUI_ModuleFunction_Pressurised(this, module->GetGui());
}


IMS_ModuleFunction_Pressurised::IMS_ModuleFunction_Pressurised(IMS_ModuleFunctionData_Pressurised *_data, IMS_Module *_module, FUNCTIONTYPE _type, 
	vector<LOCATION_CONTEXT> locationContexts, bool createGui)
	: IMS_ModuleFunction_Location(_data, _module, _type, locationContexts, _data->GetVolume())
{
	if (createGui)
	{
		menu = new GUI_ModuleFunction_Pressurised(this, module->GetGui());
	}
}


IMS_ModuleFunction_Pressurised::~IMS_ModuleFunction_Pressurised()
{
}


bool IMS_ModuleFunction_Pressurised::ProcessEvent(Event_Base *e)
{
	return false;
}

double IMS_ModuleFunction_Pressurised::GetMass()
{
	double currentmass = GetMovableMass();
	
	return currentmass;
}

IMS_Module *IMS_ModuleFunction_Pressurised::GetModule()
{
	return module;
}

GUI_ModuleFunction_Base *IMS_ModuleFunction_Pressurised::GetGui()
{
	return menu;
}