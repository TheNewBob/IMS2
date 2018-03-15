#include "GUI_Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Location.h"
#include "GUI_elements.h"
#include "IMS_Component_UI.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "LayoutManager.h"

const string LAYOUTNAME = "modulefunctions/components.xml";
const string TITLE = "title";
const string MAX_VOLUME_LBL = "max_volume_lbl";
const string AVAILABLE_VOLUME_LBL = "available_volume_lbl";
const string COMPONENT_MASS_LBL = "component_mass_lbl";
const string ADD_COMPONENT_BTN = "add_btn";
const string REMOVE_COMPONENT_BTN = "remove_btn";
const string COMPONENTS_LBL = "components_lbl";
const string COMPONENTS_LIST = "components_list";

IMS_Component_UI::IMS_Component_UI(IMS_ModuleFunction_Location *modFunction, GUIplugin *gui, RECT mRect, int _id, GUI_ElementStyle *_style, bool drawbackground = false)
	: GUI_Page(mRect, _id, _style, drawbackground)
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(LAYOUTNAME);

	gui->CreateLabel(GetLayoutDataForElement(TITLE, l), "Components", _id);
	maxVolume = gui->CreateLabelValuePair(GetLayoutDataForElement(MAX_VOLUME_LBL, l), "max volume:", 
		Helpers::doubleToString(modFunction->GetMaxVolume()) + " m3", _id);
	availableVolume = gui->CreateLabelValuePair(GetLayoutDataForElement(AVAILABLE_VOLUME_LBL, l), "available volume:", 
		Helpers::doubleToString(modFunction->GetAvailableVolume()) + " m3", _id);
	maxVolume = gui->CreateLabelValuePair(GetLayoutDataForElement(COMPONENT_MASS_LBL, l), "component mass:",
		Helpers::doubleToString(modFunction->GetComponentMass()) + "kg", _id);

	addComponentBtn = gui->CreateDynamicButton(GetLayoutDataForElement(ADD_COMPONENT_BTN, l), "add", _id);
	removeComponentBtn = gui->CreateDynamicButton(GetLayoutDataForElement(ADD_COMPONENT_BTN, l), "remove", _id);
	removeComponentBtn->SetVisible(false);

	// create a Listbox listing all components

	gui->CreateLabel(GetLayoutDataForElement(COMPONENTS_LBL, l), "installed components", _id);
	componentList = gui->CreateListBox(GetLayoutDataForElement(COMPONENTS_LIST, l), _id);
	
}


IMS_Component_UI::~IMS_Component_UI()
{
}
