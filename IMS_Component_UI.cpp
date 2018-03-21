#include "Common.h"					// TODO: Common and GUI_Common are not supposed to be included together. Clean up this mess.
#include "GUI_Common.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Location.h"
#include "IMS_Module.h"
#include "IMS_ModuleFunction_Base.h"
#include "IMS_ModuleFunction_Location.h"
#include "GUI_elements.h"
#include "IMS_Component_UI.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "GUIpopup.h"
#include "IMS_ComponentSelector.h"
#include "LayoutManager.h"
#include "IMS_Component_Model_Base.h"
#include "IMS_Movable.h"
#include "IMS_Component_Base.h"

const string LAYOUTNAME = "misc/components.xml";
const string TITLE = "title";
const string MAX_VOLUME_LBL = "max_volume_lbl";
const string AVAILABLE_VOLUME_LBL = "available_volume_lbl";
const string COMPONENT_MASS_LBL = "component_mass_lbl";
const string ADD_COMPONENT_BTN = "add_btn";
const string REMOVE_COMPONENT_BTN = "remove_btn";
const string COMPONENTS_LBL = "components_lbl";
const string COMPONENTS_LIST = "components_list";

IMS_Component_UI::IMS_Component_UI(IMS_ModuleFunction_Location *modFunction, GUIplugin *gui, RECT mRect, int _id, int parent_id, GUI_ElementStyle *_style, bool drawbackground)
	: GUI_Page(mRect, _id, _style, drawbackground), moduleFunction(modFunction)
{
	gui->RegisterGuiElement(this, parent_id);

	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(LAYOUTNAME);

	maxVolume = gui->CreateLabelValuePair(GetLayoutDataForElement(MAX_VOLUME_LBL, l), "max volume:", "", _id);
	availableVolume = gui->CreateLabelValuePair(GetLayoutDataForElement(AVAILABLE_VOLUME_LBL, l), "available volume:", "", _id);
	componentMass = gui->CreateLabelValuePair(GetLayoutDataForElement(COMPONENT_MASS_LBL, l), "component mass:", "", _id);

	addComponentBtn = gui->CreateDynamicButton(GetLayoutDataForElement(ADD_COMPONENT_BTN, l), "add", _id);
	removeComponentBtn = gui->CreateDynamicButton(GetLayoutDataForElement(ADD_COMPONENT_BTN, l), "remove", _id);
	removeComponentBtn->SetVisible(false);

	// create a Listbox listing all components
	gui->CreateLabel(GetLayoutDataForElement(COMPONENTS_LBL, l), "Installed components", _id);
	componentList = gui->CreateListBox(GetLayoutDataForElement(COMPONENTS_LIST, l), _id);

	Refresh();
}


IMS_Component_UI::~IMS_Component_UI()
{
}

int IMS_Component_UI::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	//no children of this element have been clicked
	if (eventId == -1) return -1;

	if (eventId == addComponentBtn->GetId())
	{
		vector<IMS_Component_Model_Base*> models;
		moduleFunction->GetAddableComponentModels(models);
		GUImanager::AddPopup(new IMS_ComponentSelector(
			models,
			(IMS_Location*)moduleFunction,
			this,
			_R(0, 0, width, height), 
			moduleFunction->GetModule()->GetGui()->GetStyleSet(),
			[this](vector<IMS_Component_Model_Base*> models) {
				for (UINT i = 0; i < models.size(); ++i)
				{
					if (moduleFunction->GetAvailableVolume() > models[i]->GetVolume())
					{
						moduleFunction->CreateComponent(models[i]->GetName());
						Refresh();
					}
					else
					{
						break;
					}
				}
				return true;
			}), this);
	}
}

void IMS_Component_UI::Refresh()
{
	maxVolume->SetValue(Helpers::doubleToString(moduleFunction->maxVolume, 3) + " m3");
	availableVolume->SetValue(Helpers::doubleToString(moduleFunction->availableVolume, 3) + " m3");
	componentMass->SetValue(Helpers::doubleToString(moduleFunction->componentMass, 3) + " kg");

	componentList->clear();
	for (UINT i = 0; i < moduleFunction->components.size(); ++i)
	{
		componentList->AddElement(moduleFunction->components[i]->GetName());
	}
	updatenextframe = true;
}
