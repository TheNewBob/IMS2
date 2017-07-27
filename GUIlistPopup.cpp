#include "GUI_Common.h"
#include "Events.h"
#include "GUI_Surface.h"
#include "GUI_elements.h"
#include "IMS.h"
#include "GUI_Looks.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "GUIpopup.h"
#include "GUIlistPopup.h"


GUIlistPopup::GUIlistPopup(string title, vector<string> items, GUI_BaseElement *parent, RECT rect, bool(*callback)(int, void*), void *usrdata, string styleid)
	: GUIpopup(parent, rect, styleid)
{
	CreateLabel(title, _R(10, 10, width - 10, 35), POPUP);
	
	
	//calculate the height of the listbox
	int listheight = min(height - 80, GUI_ListBox::GetNeededListBoxHeight((int)items.size(), STYLE_LISTBOX));
	list = CreateListBox(_R(10, 40, width - 10, 40 + listheight), POPUP, LISTPOPUPLIST);

	//add elements to the list
	for (UINT i = 0; i < items.size(); ++i)
	{
		list->AddElement(items[i]);
	}
	list->SetSelected(0);

	//create ok and cancel buttons
	okbtn = CreateDynamicButton("Ok", _R(width - 140, height - 35, width - 80, height - 10), POPUP, LISTPOPUPOK);
	cancelbtn = CreateDynamicButton("Cancel", _R(width - 70, height - 35, width - 10, height - 10), POPUP, LISTPOPUPCANCEL);

	clbk = callback;
	this->usrdata = usrdata;
}


GUIlistPopup::~GUIlistPopup()
{
}


bool GUIlistPopup::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (isvisible)
	{
		//see if any children have been clicked
		int eventid = -1;
		for (UINT i = rootelements.size(); i > 0; --i)
		{
			eventid = rootelements[i - 1]->ProcessMouse(_event, _x, _y);
			if (eventid != -1) break;
		}

		if (eventid != -1)
		{
			if (eventid == LISTPOPUPOK)
			{
				//invoke the callback function and tell it what item was selected
				if (clbk(list->GetSelected(), usrdata))
				{
					//if the callback returns true, close the popup
					close();
				}
			}
			else if (eventid == LISTPOPUPCANCEL)
			{
				//invoke the callback function and tell it no item was selected
				clbk(-1, usrdata);
				//close the popup regardless of what the callback returned
				close();
			}
		}
		//something has processed an event, and even if we don't care about it here, we still need a redraw
		return true;
	}

	return false;
}