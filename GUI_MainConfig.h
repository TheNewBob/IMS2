#pragma once

//a modules list for the main menu, mainly for testing purposes until we get the main engineering screen working

class GUI_MainConfig :
	public GUI_Page
{
public:
	GUI_MainConfig(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel);
	~GUI_MainConfig();

	void UpdateModulesList();

private:
	IMS2 *vessel;
	GUI_CheckBox *rcs_assist;
	GUI_CheckBox *scened_assist;
	
	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
};
