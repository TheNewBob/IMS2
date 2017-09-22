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
	rootbutton = gui->CreateDynamicButton(getLayoutDataForElement(ROOT_BTN, layouts), "X", id, GUI_MAIN_ROOT_BTN);
	rootbutton->SetVisible(false);
	//declare root menu. Add new submenu points here.
	rootmenu = gui->CreatePage(getLayoutDataForElement(MAIN_MNU, layouts), id, GUI_MAIN_ROOT_MNU);
	activemenu = rootmenu;
	//buttons that bring up the sub menus
	gui->CreateDynamicButton(getLayoutDataForElement(ASSEMBLY_BTN, layouts), "Assembly", GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_CONSTMNU_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(DECONSTRUCT_BTN, layouts), "Disassembly", GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_DECONSTMNU_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(MODULES_BTN, layouts), "Module Control", GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_MODULES_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(CONFIG_BTN, layouts), "General Configuration", GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_CONFIGMNU_BTN);

	//construction menu
	LAYOUTDATA layoutdata = getLayoutDataForElement(ASSEMBLY_PG, layouts, ignorepages);
	construct = new GUI_MainConstruct(layoutdata.rect, gui->GetStyle(layoutdata.styleId), vessel);
	construct->SetVisible(false);
	layoutdata = getLayoutDataForElement(DECONSTRUCT_PG, layouts, ignorepages);
	deconstruct = new GUI_MainDeconstruct(layoutdata.rect, gui->GetStyle(layoutdata.styleId), vessel);
	deconstruct->SetVisible(false);
	layoutdata = getLayoutDataForElement(MODULES_PG, layouts, ignorepages);
	modulescontrol = new GUI_MainModules(layoutdata.rect, gui->GetStyle(layoutdata.styleId), vessel);
	modulescontrol->SetVisible(false);
	layoutdata = getLayoutDataForElement(CONFIG_PG, layouts, ignorepages);
	generalconfig = new GUI_MainConfig(layoutdata.rect, gui->GetStyle(layoutdata.styleId), vessel);
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