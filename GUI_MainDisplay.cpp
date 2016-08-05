#include "GUI_Common.h"
#include "Events.h"
#include "GUI_Surface.h"
#include "GUI_elements.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "IMS.h"
#include "IMS_InputCallback.h"
#include "GUI_MainConstruct.h"
#include "GUI_MainDeconstruct.h"
#include "GUI_MainModules.h"
#include "GUI_MainDisplay.h"

GUI_MainDisplay::GUI_MainDisplay(IMS2 *_vessel, GUI_ElementStyle *_style, int _panelid)
: GUI_Surface(_vessel, _style, _panelid)
{
	
}

GUI_MainDisplay::~GUI_MainDisplay(void)
{
}

//create children
void GUI_MainDisplay::PostInit()
{

	//menu size will be the same for all menus
	RECT menurect = _R(0, 40, width, height - 10);
	GUImanager *gui = vessel->GetGUI();
	//button that leads back to the root menu. always visible except in root menu
	rootbutton = gui->CreateDynamicButton("X", _R(width - 40, 10, width - 10, 35), GUI_MAIN_DISPLAY, GUI_MAIN_ROOT_BTN, STYLE_BOLD_BUTTON);
	rootbutton->SetVisible(false);
	//declare root menu. Add new submenu points here.
	rootmenu = gui->CreatePage(menurect, GUI_MAIN_DISPLAY, GUI_MAIN_ROOT_MNU);
	activemenu = rootmenu;
	//buttons that bring up the sub menus
	gui->CreateDynamicButton("Assembly", _R(50, 10, width - 50, 35), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_CONSTMNU_BTN);
	gui->CreateDynamicButton("Disassembly", _R(50, 50, width - 50, 75), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_DECONSTMNU_BTN);
	gui->CreateDynamicButton("Module Control", _R(50, 90, width - 50, 115), GUI_MAIN_ROOT_MNU, GUI_MAIN_ROOT_MODULES_BTN);

	//construction menu
	construct = new GUI_MainConstruct(menurect, style, vessel);
	construct->SetVisible(false);
	deconstruct = new GUI_MainDeconstruct(menurect, style, vessel);
	deconstruct->SetVisible(false);
	modulescontrol = new GUI_MainModules(menurect, style, vessel);
	modulescontrol->SetVisible(false);

}

bool GUI_MainDisplay::ProcessMe(int eventId)
{
	if (eventId == -1) return false;

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
	}
	return true;
}


void GUI_MainDisplay::UpdateDockedVesselsList(std::vector<DOCKEDVESSEL*> &vesselList)
{
	if (!isInitialised)
	{
		return;
	}
	construct->UpdateDockedVesselsList(vesselList);
	vessel->GetGUI()->RedrawGUISurface(id);
}


void GUI_MainDisplay::UpdateModulesList()
{
	if (!isInitialised)
	{
		return;
	}
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
	if (*e == VESSELLAYOUTCHANGEDEVENT)
	{
		modulescontrol->UpdateModulesList();
	}

	return false;
}