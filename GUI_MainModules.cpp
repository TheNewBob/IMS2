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


GUI_MainModules::GUI_MainModules(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_MODULES_MENU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();

	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	//the close button that leads back from a module's page to the module selection page
	closebtn = gui->CreateDynamicButton("back", _R(width - 80, 10, width - 10, 35), MD_MODULES_MENU, MD_MODULES_ROOT_BTN, STYLE_BOLD_BUTTON);
	closebtn->SetVisible(false);

	RECT menurect = _R(0, 0, width, height);

	//create a page that will contain the module selection
	mainpage = gui->CreatePage(menurect, MD_MODULES_MENU, MD_MODULES_MODSELECTPG);
	//create a page that will contain all module controls
	modcontrolpage = gui->CreatePage(_R(10, 40, rect.right - 10, height), MD_MODULES_MENU, MD_MODULES_CTRLPG);
	modcontrolpage->SetVisible(false);

	//create a listbox listing all the modules
	gui->CreateLabel("modules in vessel", _R(30, 0, int(width * 0.6), 25), MD_MODULES_MODSELECTPG);
	moduleslist = gui->CreateListBox(_R(30, 30, int(width * 0.6), 280), MD_MODULES_MODSELECTPG, MD_MODULES_MODLIST);

	//create a button to switch to the menu page of the currently selected module
	gui->CreateDynamicButton("show", _R(int(width * 0.6) + 10, 30, int(width * 0.6) + 130, 55), MD_MODULES_MODSELECTPG, MD_MODULES_MODSELECTBTN);

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