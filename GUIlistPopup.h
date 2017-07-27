#pragma once


/**
 * \brief ids for the popups elements
 */

enum
{
	LISTPOPUPLIST = 2,
	LISTPOPUPOK,
	LISTPOPUPCANCEL
};

/**
 * Presents a list to select from, with a cancel and ok button.
 */
class GUIlistPopup :
	public GUIpopup
{
public:
	GUIlistPopup(string title, vector<string> items, GUI_BaseElement *parent, RECT rect, bool(*callback)(int, void*), void *usrdata = NULL, string styleid = STYLE_DEFAULT);
	~GUIlistPopup();
	bool ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

private:
	GUI_ListBox *list;
	GUI_DynamicButton *okbtn;
	GUI_DynamicButton *cancelbtn;

	bool(*clbk)(int, void*);				//!< callback function pointer
	void *usrdata;							//!< the user data that will be passed to the callback

};

