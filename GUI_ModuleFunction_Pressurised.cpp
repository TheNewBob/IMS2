#include "GUI_Common.h"
#include "Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "IMS_Location.h"
#include "Moduletypes.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "IMS_ModuleFunction_Location.h"
#include "IMS_ModuleFunction_Pressurised.h"
#include "GUI_ModuleFunction_Base.h"
#include "GUI_ModuleFunction_Pressurised.h"
#include "LayoutManager.h"
#include "IMS_Component_UI.h"

static const string LAYOUTNAME = "modulefunctions/pressurised.xml";
const static string NAME_LBL = "name_lbl";
const static string COMPONENTS_VIEW = "components_view";

GUI_ModuleFunction_Pressurised::GUI_ModuleFunction_Pressurised(IMS_ModuleFunction_Pressurised *moduleFunction, GUIplugin *gui)
	:GUI_ModuleFunction_Base(LayoutManager::GetLayoutHeight(LAYOUTNAME, predictWidth(gui, INT_MAX)), gui, gui->GetStyle(IMS_ModuleFunction_Base::GUI_MODULE_STYLE)),
	function(moduleFunction)
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(LAYOUTNAME);
	auto test = gui->CreateLabel(GetLayoutDataForElement(NAME_LBL, l), moduleFunction->data->GetName(), id);
	auto layoutdata = GetLayoutDataForElement(COMPONENTS_VIEW, l);
	componentsView = new IMS_Component_UI(
		moduleFunction, 
		gui, 
		layoutdata.rect, 
		gui->GetDynamicUID(),
		id, gui->GetStyle(layoutdata.styleId));

//	updatenextframe = true;
}


GUI_ModuleFunction_Pressurised::~GUI_ModuleFunction_Pressurised()
{
}

int GUI_ModuleFunction_Pressurised::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	return GUI_BaseElement::ProcessChildren(_event, _x, _y);
}

void GUI_ModuleFunction_Pressurised::RefreshComponents()
{
	componentsView->Refresh();
}

/*bool GUI_ModuleFunction_Pressurised::updateMe()
{
	return true;
}*/