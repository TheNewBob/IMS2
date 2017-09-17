#pragma once

class GUI_MainConstruct;
class GUI_MainDeconstruct;
class GUI_MainModules;
class GUI_MainConfig;

//the main display to the right of the engineering panel

class GUI_MainDisplay :
	public IMS_RootPage
{
public:
	GUI_MainDisplay(IMS2 *vessel, RECT rect, GUI_ElementStyle *style);
	~GUI_MainDisplay(void);
	void PostConstruction(GUIentity *gui);
	bool ProcessMouse2D (GUI_MOUSE_EVENT _event, int mx, int my);
	void UpdateDockedVesselsList(vector<DOCKEDVESSEL*> &vesselList);
	void UpdateModulesList();

private:
	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	void switchMenu(GUI_BaseElement *switch_to);

	GUI_DynamicButton *rootbutton;
	GUI_Page *rootmenu;
	GUI_BaseElement *activemenu;

	GUI_MainConstruct *construct;
	GUI_MainDeconstruct *deconstruct;
	GUI_MainModules *modulescontrol;
	GUI_MainConfig *generalconfig;

	//overload from EventHandler
	bool ProcessEvent(Event_Base *e);

	IMS2 *vessel;
	
};
