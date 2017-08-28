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
#include "GUIalert.h"


GUIalert::GUIalert(string text, GUI_BaseElement *parent, RECT rect, string styleid)
	: GUIpopup(parent, rect, styleid)
{
	//create the text
	GUI_ElementStyle *style = GUI_Looks::GetStyle(styleid);
	RECT textrect = _R(style->MarginLeft(), style->MarginTop(), width - style->MarginRight(), style->MarginTop() + style->GetFont()->GetfHeight() + 4);
	CreateLabel(text, textrect, POPUP, ALERTTEXT, styleid);

	//create the ok button
	RECT okrect = _R(width / 2 - 20, height - style->MarginBottom() - style->GetFont()->GetfHeight() - 10,
		width / 2 + 20, height - style->MarginBottom());

	CreateDynamicButton("OK", okrect, POPUP, ALERTOKBTN);
}


GUIalert::~GUIalert()
{
}



bool GUIalert::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (isvisible)
	{
		int eventid = -1;
		for (UINT i = rootelements.size(); i > 0; --i)
		{
			eventid = rootelements[i - 1]->ProcessMouse(_event, _x, _y);
			if (eventid != -1) break;
		}

		if (eventid == ALERTOKBTN)
		{
			close();
			return true;
		}
	}
	return false;
}







