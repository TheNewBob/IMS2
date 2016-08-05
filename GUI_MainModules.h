#pragma once

//a modules list for the main menu, mainly for testing purposes until we get the main engineering screen working

class GUI_MainModules :
	public GUI_Page
{
public:
	GUI_MainModules(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel);
	~GUI_MainModules();

	void UpdateModulesList();

private:
	IMS2 *vessel;
	GUI_ListBox *moduleslist;
	vector<IMS_Module*> modules;
	vector<UINT> modulesortedorder;
	GUI_Page *mainpage;
	GUI_Page *modcontrolpage;
	GUI_DynamicButton *closebtn;

	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	
	/* brings up the page with the selected module gui
	 */
	void showModuleGui();
	
	/* hides the currently displayed module controls
	 * and displays the main page again 
	 */
	void showMainPage();
};




