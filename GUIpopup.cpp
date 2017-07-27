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


GUIpopup::GUIpopup(GUI_BaseElement *parent, RECT rect, string styleid)
{
	//calculate width and height of the popup
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	//create the page that displays the popup
	GUI_Page *popup = CreatePage(rect, -1, POPUP, styleid, true);

	isvisible = true;
	isfixed = true;
}


GUIpopup::~GUIpopup()
{
}


void GUIpopup::close()
{
	isvisible = false;
	GUImanager::ClearPopup(this);
}

