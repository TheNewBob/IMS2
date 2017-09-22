#include "Common.h"
#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUI_MainConstruct.h"
#include "GUIentity.h"
#include "GUIplugin.h"
#include "GUImanager.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "GUI_Module.h"
#include "IMS_InputCallback.h"
#include "GUI_MainModules.h"
#include "LayoutManager.h"

const static string FILENAME = "mainmenu/modules.xml";
const static string BACK_BTN = "back_btn";
const static string MAIN_PAGE = "main_page";
const static string MODULES_IN_VESSEL_LABEL = "modules_in_vessel_label";
const static string MODULES_LIST = "modules_list";
const static string SHOW_MODULE_BTN = "show_module_btn";


GUI_MainModules::GUI_MainModules(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_MODULES_MENU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();
	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);

	//the close button that leads back from a module's page to the module selection page
	closebtn = gui->CreateDynamicButton(getLayoutDataForElement(BACK_BTN, layouts), "back", MD_MODULES_MENU, MD_MODULES_ROOT_BTN);
	closebtn->SetVisible(false);

	//create a page that will contain the module selection
	mainpage = gui->CreatePage(getLayoutDataForElement(MAIN_PAGE, layouts), MD_MODULES_MENU, MD_MODULES_MODSELECTPG);
	//create a listbox listing all the modules
	gui->CreateLabel(getLayoutDataForElement(MODULES_IN_VESSEL_LABEL, layouts), "modules in vessel", MD_MODULES_MODSELECTPG);
	moduleslist = gui->CreateListBox(getLayoutDataForElement(MODULES_LIST, layouts), MD_MODULES_MODSELECTPG, MD_MODULES_MODLIST);
	//create a button to switch to the menu page of the currently selected module
	gui->CreateDynamicButton(getLayoutDataForElement(SHOW_MODULE_BTN, layouts), "show", MD_MODULES_MODSELECTPG, MD_MODULES_MODSELECTBTN);

	//create a page that will contain all module controls
	//this is a dynamically assembled listview. It uses the same space as the main page, but we automatically stretch it to use the full height.
	LAYOUTDATA listviewdata = getLayoutDataForElement(MAIN_PAGE, layouts);
	listviewdata.rect.bottom = height;
	modcontrolpage = gui->CreatePage(listviewdata.rect, MD_MODULES_MENU, MD_MODULES_CTRLPG);
	modcontrolpage->SetVisible(false);
}



GUI_MainModules::~GUI_MainModules()
{
}



void GUI_MainModules::UpdateModulesList()
{
	//recreate the list of the actual module pointers
	modules.clear();
	vessel->GetModules(modules);
	
	//remove modules without a GUI from the list
	for (UINT i = modules.size(); i > 0; --i)
	{
		if (modules[i - 1]->GetGui() == NULL)
		{
			modules.erase(modules.begin() + (i - 1));
		}
	}

	//sort the modules by symmetric z. There will be more convenient ways to access modules later on, so let's not spend too much effort on this
	sort(modules.begin(), modules.end(), IMS_Module::Z_SYMMETRIC_ORDER);
	
	//add the modules to the listbox
	moduleslist->clear();
	for (UINT i = 0; i < modules.size(); ++i)
	{
		moduleslist->AddElement(modules[i]->getModulePosDescription());
	}
}



int GUI_MainModules::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//get the clicked element
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	if (eventId == -1) return -1;

	//handle results of click events
	switch (eventId)
	{
	case  MD_MODULES_MODSELECTBTN:
		showModuleGui();
		break;
	case MD_MODULES_ROOT_BTN:
		showMainPage();
		break;
	}

	return eventId;
}



void GUI_MainModules::showModuleGui()
{
	//get the selected item from the listbox
	int idx = moduleslist->GetSelected();
	//check if the listbox even has a selected element
	if (idx != -1)
	{
		//hide the page for selecting the module and display the module's own page
		mainpage->SetVisible(false);
		closebtn->SetVisible(true);
		modcontrolpage->SetVisible(true);
		modules[idx]->GetGui()->SetVisible(true);
	}
}



void GUI_MainModules::showMainPage()
{
	//get the selected item from the listbox
	int idx = moduleslist->GetSelected();
	//check if the listbox even has a selected element
	if (idx != -1)
	{
		//hide the module page 
		modules[idx]->GetGui()->SetVisible(false);
	}
	modcontrolpage->SetVisible(false);
	closebtn->SetVisible(false);
	mainpage->SetVisible(true);
}