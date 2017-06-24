#pragma once

//a modules list for the main menu, mainly for testing purposes until we get the main engineering screen working

class GUI_MainRootMenu :
	public GUI_LayoutPage
{
public:
	GUI_MainRootMenu(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel);
	~GUI_MainRootMenu();

	void UpdateModulesList();

private:
	IMS2 *vessel;

	GUI_DynamicButton *rootbutton;
	GUI_Page *rootmenu;
	GUI_BaseElement *activemenu;

	GUI_MainConstruct *construct;
	GUI_MainDeconstruct *deconstruct;
	GUI_MainModules *modulescontrol;
	GUI_MainConfig *generalconfig;

	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
};
