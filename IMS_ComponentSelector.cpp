#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "GUIpopup.h"
#include "Components.h"
#include "LayoutManager.h"
#include "IMS_ComponentSelector.h"
#include "IMS_Location.h"


const string LAYOUTFILE = "popups/componentselector.xml";
const string TITLE_ID = "title";
const string LIST_ID = "components_list";
const string OKBTN_ID = "ok_btn";
const string CANCELBTN_ID = "cancel_btn";

IMS_ComponentSelector::IMS_ComponentSelector(vector<IMS_Component_Model_Base*> components, IMS_Location *location, GUI_BaseElement *parent,
	RECT rect, string styleset, std::function<bool(vector<IMS_Component_Model_Base*>)> callback)
	: GUIpopup(parent, rect, "component_selector", styleset), callback(callback)
{
	LAYOUTCOLLECTION *l = LayoutManager::GetLayout(LAYOUTFILE);
	CreateLabel(page->GetLayoutDataForElement(TITLE_ID, l), "Select Components", POPUP);

	//calculate the height of the listbox and make it smaller if it doesn't fit within the rect.
	LAYOUTDATA boxData = page->GetLayoutDataForElement(LIST_ID, l);
	LAYOUTDATA okBtnData = page->GetLayoutDataForElement(OKBTN_ID, l);
	LAYOUTDATA cancelBtnData = page->GetLayoutDataForElement(CANCELBTN_ID, l);
	int btnHeight = okBtnData.GetHeight();
	int originalListBoxHeight = boxData.GetHeight();
	int listboxHeight = min((height - boxData.rect.top) - btnHeight, originalListBoxHeight);
	boxData.rect.bottom = boxData.rect.top + listboxHeight; 
	list = CreateListBox(boxData, POPUP, true);
	
	//place the buttons
	int listHeightDifference = originalListBoxHeight - boxData.GetHeight();
	okBtnData.rect.top = okBtnData.rect.top - listHeightDifference;
	okBtnData.rect.bottom = okBtnData.rect.bottom - listHeightDifference;
	cancelBtnData.rect.top = cancelBtnData.rect.top - listHeightDifference;
	cancelBtnData.rect.bottom = cancelBtnData.rect.bottom - listHeightDifference;

	okbtn = CreateDynamicButton(okBtnData, "Ok", POPUP);
	cancelbtn = CreateDynamicButton(cancelBtnData, "Cancel", POPUP);

	//add elements to the list
	for (UINT i = 0; i < components.size(); ++i)
	{
		// TODO: Filter by context compatibility of component and location.
		list->AddElement(components[i]->GetName());
	}
	list->SetSelected(0);
}


IMS_ComponentSelector::~IMS_ComponentSelector()
{
}

bool IMS_ComponentSelector::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (isvisible)
	{
		//see if any children have been clicked
		int eventid = -1;
		for (UINT i = rootelements.size(); i > 0; --i)
		{
			eventid = rootelements[i - 1]->ProcessMouse(_event, _x, _y);
			if (eventid != -1) break;
		}

		if (eventid != -1)
		{
			if (eventid == okbtn->GetId())
			{
				// make a list with the selected component models, and pass them to the callback
				vector<IMS_Component_Model_Base*> selectedComponents;
				UINT listSize = list->NumEntries();
				for (UINT i = 0; i < listSize; ++i)
				{
					if (list->IsHighlight(i)) selectedComponents.push_back(data[i]);
					if (callback(selectedComponents)) close();
				}
			}
			else if (eventid == cancelbtn->GetId()) close();
			return true;
		}
	}

	return false;
}