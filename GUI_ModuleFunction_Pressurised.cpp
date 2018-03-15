#include "GUI_Common.h"
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
#include "IMS_ModuleFunction_Pressurised.h"
#include "GUI_ModuleFunction_Base.h"
#include "GUI_ModuleFunction_Pressurised.h"
#include "LayoutManager.h"
#include "IMS_Component_UI.h"

static const string LAYOUTNAME = "modulefunctions/pressurised.xml";
const static string NAME_LBL = "name_lbl";
const static string COMPONENTS_VIEW = "components_view";

GUI_ModuleFunction_Pressurised::GUI_ModuleFunction_Pressurised(IMS_ModuleFunction_Pressurised *moduleFunction, GUIplugin *gui)
	:GUI_ModuleFunction_Base(LayoutManager::GetLayoutHeight(LAYOUTNAME, predictWidth(gui, INT_MAX)), gui, gui->GetStyle(IMS_ModuleFunction_Base::GUI_MODULE_STYLE))
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(LAYOUTNAME);
	gui->CreateLabel(GetLayoutDataForElement(NAME_LBL, l), moduleFunction->data->GetName(), id);
	auto layoutdata = GetLayoutDataForElement(COMPONENTS_VIEW, l);
	auto componentsView = IMS_Component_UI(
		moduleFunction, 
		gui, 
		layoutdata.rect, 
		gui->GetDynamicUID(),
		gui->GetStyle(layoutdata.styleId));
}


GUI_ModuleFunction_Pressurised::~GUI_ModuleFunction_Pressurised()
{
}
