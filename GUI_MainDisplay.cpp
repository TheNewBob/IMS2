#include "Common.h"
#include "GUI_Common.h"
#include "Events.h"
#include "GUI_elements.h"
#include "IMS_RootPage.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "IMS.h"
#include "IMS_InputCallback.h"
#include "GUI_MainConstruct.h"
#include "GUI_MainDeconstruct.h"
#include "GUI_MainModules.h"
#include "GUI_MainConfig.h"
#include "GUI_MainDisplay.h"
#include "LayoutManager.h"
#include "GUI_Looks.h"

const string FILENAME = "mainmenu/root.xml";
const string ROOT_BTN = "root_btn";
const string MAIN_MNU = "main_menu";
const string ASSEMBLY_BTN = "assembly_btn";
const string DECONSTRUCT_BTN = "deconstruct_btn";
const string MODULES_BTN = "modules_btn";
const string CONFIG_BTN = "config_btn";
const string ASSEMBLY_PG = "assembly_pg";
const string DECONSTRUCT_PG = "deconstruct_pg";
const string MODULES_PG = "modules_pg";
const string CONFIG_PG = "config_pg";


GUI_MainDisplay::GUI_MainDisplay(IMS2 *vessel, RECT rect, GUI_ElementStyle *style)
	: IMS_RootPage(vessel, GUI_MAIN_DISPLAY, rect, style), vessel(vessel)
{
	
}

GUI_MainDisplay::~GUI_MainDisplay(void)
{
}

//create children
void GUI_MainDisplay::PostConstruction(GUIentity *gui)
{
	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);
	vector<string> ignorepages = { MAIN_MNU, ASSEMBLY_PG, DECONSTRUCT_PG, MODULES_PG, CONFIG_PG };

	//menu size will be the same for all menus
	//button that leads back to the root menu. always visible except in root menu
	rootbutton = gui->CreateDynamicButton("X", getElementRect(ROOT_BTN, layouts), id, GUI_MAIN_ROOT_BTN, getElementStyle(ROOT_BTN, layouts));
	rootbutton->SetVisible(false);
	//declare root menu. Add new submenu points here.
	rootmenu = gui->CreatePage(getElementRect(MAIN_MNU, layouts), id, GUI_MAIN_ROOT_MNU);
	activemenu = rootmenu;
	//buttons that bring up the sub menus
	gui->CreateDynamicButton("Assembly", getElementRect(ASSEMBLY_BTN, layouts), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_CONSTMNU_BTN, getElementStyle(ASSEMBLY_BTN, layouts));
	gui->CreateDynamicButton("Disassembly", getElementRect(DECONSTRUCT_BTN, layouts), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_DECONSTMNU_BTN, getElementStyle(DECONSTRUCT_BTN, layouts));
	gui->CreateDynamicButton("Module Control", getElementRect(MODULES_BTN, layouts), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_MODULES_BTN, getElementStyle(MODULES_BTN, layouts));
	gui->CreateDynamicButton("General Configuration", getElementRect(CONFIG_BTN, layouts), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_CONFIGMNU_BTN, getElementStyle(CONFIG_BTN, layouts));

	//construction menu
	construct = new GUI_MainConstruct(getElementRect(ASSEMBLY_PG, layouts, ignorepages), gui->GetStyle(getElementStyle(ASSEMBLY_PG, layouts)), vessel);
	construct->SetVisible(false);
	deconstruct = new GUI_MainDeconstruct(getElementRect(DECONSTRUCT_PG, layouts, ignorepages), gui->GetStyle(getElementStyle(DECONSTRUCT_PG, layouts)), vessel);
	deconstruct->SetVisible(false);
	modulescontrol = new GUI_MainModules(getElementRect(MODULES_PG, layouts, ignorepages), gui->GetStyle(getElementStyle(MODULES_PG, layouts)), vessel);
	modulescontrol->SetVisible(false);
	generalconfig = new GUI_MainConfig(getElementRect(CONFIG_PG, layouts, ignorepages), gui->GetStyle(getElementStyle(CONFIG_PG, layouts)), vessel);
	generalconfig->SetVisible(false);
}

int GUI_MainDisplay::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//get the clicked element
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);
	if (eventId == -1) return -1;

	switch (eventId)
	{
	case -1: return false;
		break;
	case GUI_MAIN_ROOT_BTN:
		switchMenu(rootmenu);
		break;
	case GUI_MAIN_ROOT_CONSTMNU_BTN:
		switchMenu(construct);
		break;
	case GUI_MAIN_ROOT_DECONSTMNU_BTN:
		switchMenu(deconstruct);
		break;
	case GUI_MAIN_ROOT_MODULES_BTN:
		switchMenu(modulescontrol);
		break;
	case GUI_MAIN_ROOT_CONFIGMNU_BTN:
		switchMenu(generalconfig);
		break;

	}
	return true;
}


void GUI_MainDisplay::UpdateDockedVesselsList(std::vector<DOCKEDVESSEL*> &vesselList)
{
	construct->UpdateDockedVesselsList(vesselList);
	vessel->GetGUI()->RedrawGUISurface(id);
}


void GUI_MainDisplay::UpdateModulesList()
{
	deconstruct->UpdateModulesList();
	vessel->GetGUI()->RedrawGUISurface(id);
}


void GUI_MainDisplay::switchMenu(GUI_BaseElement *switch_to)
{
	activemenu->SetVisible(false);
	activemenu = switch_to;
	activemenu->SetVisible(true);

	if (activemenu == rootmenu) rootbutton->SetVisible(false);
	else rootbutton->SetVisible(true);
}


bool GUI_MainDisplay::ProcessEvent(Event_Base *e)
{
	if (*e == VESSELLAYOUTCHANGEDEVENT || *e == SIMULATIONSTARTEDEVENT)
	{
		modulescontrol->UpdateModulesList();
	}

	return false;
}