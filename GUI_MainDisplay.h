#pragma once

class GUI_MainConstruct;
class GUI_MainDeconstruct;
class GUI_MainModules;

//the main display to the right of the engineering panel

class GUI_MainDisplay :
	public GUI_Surface
{
public:
	GUI_MainDisplay(IMS2 *_vessel, GUI_ElementStyle *_style, int _panelid);
	~GUI_MainDisplay(void);
	void PostInit();
	bool ProcessMouse2D (GUI_MOUSE_EVENT _event, int mx, int my);
	void UpdateDockedVesselsList(vector<DOCKEDVESSEL*> &vesselList);
	void UpdateModulesList();

private:
	virtual bool ProcessMe(int eventId);
	void switchMenu(GUI_BaseElement *switch_to);

	GUI_DynamicButton *rootbutton;
	GUI_Page *rootmenu;
	GUI_BaseElement *activemenu;

	GUI_MainConstruct *construct;
	GUI_MainDeconstruct *deconstruct;
	GUI_MainModules *modulescontrol;

	//overload from EventHandler
	bool ProcessEvent(Event_Base *e);
	
};
