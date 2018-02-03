#include "GUI_Common.h"
#include "Events.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"
#include "GUI_elements.h"
#include "GUI_Looks.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUIplugin.h"
#include "GUIpopup.h"
#include "GUIalert.h"
#include "GUIlistPopup.h"
#include "GUI_Panel.h"


unsigned long GUImanager::lastclock = 0;
unsigned int GUImanager::managerinstances = 0;
map<GUIplugin*, GUI_BaseElement*> GUImanager::temporaryplugins;


GUImanager::GUImanager(VESSEL3 *vessel, string initialstyleset)
	: vessel(vessel)
{
	styleset = initialstyleset;
	curEvent = LBNONE;
	managerinstances++;
}

GUImanager::~GUImanager(void)
{
	for (UINT i = 0; i < surfaces.size(); ++i)
	{
		delete surfaces[i];
		surfaces[i] = NULL;
	}

	managerinstances--;
}

GUI_MOUSE_EVENT GUImanager::GetEvent(int _event)
{
	if (curEvent == LBUP) 
	{
		curEvent = LBNONE;
	}
	if (_event == PANEL_MOUSE_LBDOWN && curEvent == LBNONE ||
			_event == PANEL_MOUSE_LBPRESSED && curEvent == LBNONE) 
	{
		curEvent = LBDOWN;
	}
	else if (_event == PANEL_MOUSE_LBPRESSED && curEvent == LBDOWN ||
			 _event == PANEL_MOUSE_LBDOWN && curEvent == LBDOWN) 
	{
		curEvent = LBPRESSED;
	}
	else if (_event == PANEL_MOUSE_LBUP)
	{
		curEvent = LBUP;
	}
	return curEvent;
}


void GUImanager::RegisterGUISurface(GUI_Surface *surf, int _id, RECT surfacePos)
{

	//if the surface hasn't been registered yet in the manager, register it now
	if(!SurfaceIsRegistered(_id))
	{
		surfaces.push_back(surf);
		surf->Init(surfacePos, _id);
	}

	//let the surface do post-registration stuff, like creating children
	surf->PostInit();
}


void GUImanager::RegisterSurfaceWithOrbiter(GUI_Surface *surf, PANELHANDLE hpanel, SURFHANDLE tgt)
{
	//register the surface in Orbiter
	vessel->RegisterPanelArea(hpanel, surf->GetId(), surf->rect, 
		PANEL_REDRAW_USER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP | PANEL_MOUSE_ONREPLAY, tgt, surf);
}

void GUImanager::RegisterGUIPanel(GUI_Panel *panel)
{
	Olog::assertThat([this, panel]() { return panels.find(panel->id) == panels.end(); }, "Panel registered twice!");
	panels[panel->id] = panel;
}

bool GUImanager::IsPanelRegistered(int id)
{
	return panels.find(id) != panels.end();
}

bool GUImanager::LoadPanel(int id, PANELHANDLE hpanel)
{
	auto panelit = panels.find(id);
	
	if (panelit != panels.end())
	{
		panelit->second->loadPanel(this, hpanel, hPanelMesh, GUI_Looks::GetStyle("default", styleset), vessel);
		SetCurPanelId(id);
		RedrawCurrentPanel();
		return true;
	}

	return false;
}

bool GUImanager::Update()
{
	bool hasredrawn = false;
	//check if orbiter is in internal view and the vessel this GUI belongs to is the focus vessel,
	//if not, don't bother
	if (oapiCameraInternal() && vessel == oapiGetFocusInterface())
	{
		for (UINT i = 0; i < surfaces.size(); ++i)
		{
			if (surfaces[i]->GetPanelId() == curPanelId &&		//if the surface is not on the currently active panel, don't bother
				surfaces[i]->Update())
			{
				hasredrawn = true;
			}
		}
	}

	//check in with the synchronised clock
	registerPreStep();

	return hasredrawn;
}

int GUImanager::GetCurPanelId()
{
	return curPanelId;
}

void GUImanager::SetCurPanelId(int _curPanelId)
{
	curPanelId = _curPanelId;
}

bool GUImanager::SurfaceIsRegistered(int _id)
{
	for(UINT i = 0; i < surfaces.size(); ++i)
	{
		if (surfaces[i]->GetId() == _id)
		{
			return true;
		}
	}
	return false;
}

void GUImanager::RedrawCurrentPanel()
{
	//check whether the vessel actually has focus and is in internal camera. Otherwise a redraw is just a waste of cycles
	if (oapiCameraInternal() && vessel == oapiGetFocusInterface())
	{
		for(UINT i = 0; i < surfaces.size(); ++i)
		{
			if (surfaces[i]->GetPanelId() == curPanelId)
			{
				vessel->TriggerPanelRedrawArea(curPanelId, surfaces[i]->GetId());
			}
		}
	}
}

void GUImanager::RedrawGUISurface(int surfId)
{
	//check whether the vessel actually has focus and is in internal camera. Otherwise a redraw is just a waste of cycles
	if (oapiCameraInternal() && vessel == oapiGetFocusInterface())
	{
		vessel->TriggerPanelRedrawArea(curPanelId, surfId);
	}

}





GUI_Surface *GUImanager::GetSurfaceById(int _id)
//returns the surface with the passed id, NULL if not found
{
	for (UINT i = 0; i < surfaces.size(); ++i)
	{
		if (surfaces[i]->GetId() == _id)
		{
			return surfaces[i];
		}
	}
	return NULL;
}

bool GUImanager::AddChildToParent(GUI_BaseElement *child, int parentId)
{
	GUI_Surface *parentSurf = NULL;
	GUI_BaseElement *parentElement = NULL;

	//check if an element with the passed id exists
	parentElement = GetElementById(parentId);
	if (parentElement == NULL)
	{
		return false;
	}
	else
	{
		parentElement->AddChild(child);
	}
	return true;
}


GUI_BaseElement *GUImanager::GetElementById(int _id)
{
	map<int, GUI_BaseElement*>::iterator elem = elements.find(_id);
	//the id was not found among the elements. return NULL, so the caller can go look among the surfaces
	if (elem == elements.end())
	{
		return NULL;
	}
	
	return elem->second;
}







bool GUImanager::CheckSynchronisedInterval(int interval_in_miliseconds)
{
	//checks what time it is now
	unsigned long now = (unsigned long)(oapiGetSimTime() * 1000);

	//now check if the querried interval has been passed between then and now
	bool intervalfilled = false;
	if (now % interval_in_miliseconds < lastclock % interval_in_miliseconds)
	{
		intervalfilled = true;
	}
	return intervalfilled;
}


void GUImanager::registerPreStep()
{
	//this variable keeps track of how many instances have called in this frame
	static UINT processedmanagers = 0;

	processedmanagers++;
	if (processedmanagers == managerinstances)
	{
		//all registered instances have checked in, the frame is complete.
		//update the clock
		processedmanagers = 0;
		lastclock = (unsigned long)(oapiGetSimTime() * 1000);
		//clear popups if one needs cleaning
		ClearPopup();
	}
}




void GUImanager::Alert(string text, GUI_BaseElement *parent, RECT rect, string styleid, string styleset)
{
	if (rect.left == 0 && rect.top == 0 &&
		rect.right == 0 && rect.bottom == 0)
	{
		//try to find a sensible size for the alert
		int parentwidth = parent->GetWidth();
		int parentheight = parent->GetHeight();
		int width = min(parentwidth, parent->GetStyle()->GetFont()->GetTextWidth(text) + 20);
		width = max(width, 50);
		int height = 100;
		rect.left = parentwidth / 2 - width / 2;
		rect.right = rect.left + width;
		rect.top = parentheight / 2 - height / 2;
		rect.bottom = rect.top + height;
	}
	GUIalert *alert = new GUIalert(text, parent, rect, styleid, styleset);
	parent->AddPlugin(alert);
	temporaryplugins.insert(make_pair(alert, parent));
}


void GUImanager::ListDialog(string title, vector<string> &items, GUI_BaseElement *parent, bool(*callback)(int, void*), void *usrdata, RECT rect, string styleset)
{
	if (rect.left == 0 && rect.top == 0 &&
		rect.right == 0 && rect.bottom == 0)
	{
		//try to find a sensible size for the list dialog
		int parentwidth = parent->GetWidth();
		int parentheight = parent->GetHeight();
		int width = parentwidth - 20;

		int listheight = GUI_ListBox::GetNeededListBoxHeight((int)items.size(), STYLE_LISTBOX);
		//the maximum height of the box would be a list long enough to show all elements, or the height of the parent minus borders
		int height = min(listheight + 80, parentheight - 20);
		//and then again, if that would make the list essentially unusable, make it larger than the parent!
		int minlistheight = GUI_ListBox::GetNeededListBoxHeight(min((int)items.size(), 4), STYLE_LISTBOX);
		height = max(height, minlistheight + 80);
		
		rect.left = parentwidth / 2 - width / 2;
		rect.right = rect.left + width;
		rect.top = parentheight / 2 - height / 2;
		rect.bottom = rect.top + height;
	}
	
	GUIlistPopup *popup = new GUIlistPopup(title, items, parent, rect, callback, usrdata, STYLE_DEFAULT, styleset);
	parent->AddPlugin(popup);
	temporaryplugins.insert(make_pair(popup, parent));
}



void GUImanager::ClearPopup(GUIpopup *popup)
{
	static GUIplugin *popuptoclear = NULL;

	//if an alert has been passed, remember it for the next call
	if (popup != NULL)
	{
		//if this assert is ever triggered, something is very weird. In explicit, somehow
		//it was possible for the user to prompt two popups in the same frame, which really
		//should not be possible, but never trust a user!
		//This is here purely that we would notice if somebody actually succeeded, because then
		//I'd have to make this function somewhat smarter.
		Olog::assertThat([]() { return popuptoclear == NULL; }, "Two popups prompted in same frame! Congratulations, you achieved what should have been impossible.");
		popuptoclear = popup;
	}
	else
	{
		//if no alert was passed, check if one registered for cleanup during last frame.
		if (popuptoclear != NULL)
		{
			//remove the popup from its parent element
			map<GUIplugin*, GUI_BaseElement*>::iterator i = temporaryplugins.find(popuptoclear);
			Olog::assertThat([i]() { return i != temporaryplugins.end(); },
				"somebody's trying to delete a popup that was never properly registered. DO NOT instantiate popups directly!");
			i->second->RemovePlugin(popuptoclear);
			temporaryplugins.erase(i);

			//destroy the popup
			delete popuptoclear;
			//pretend that nothing ever happened
			popuptoclear = NULL;
		}
	}
}

GUI_ElementStyle *GUImanager::GetStyle(string styleid) 
{
	return GUI_Looks::GetStyle(styleid, styleset);
}

void GUImanager::SetStyleset(string styleset)
{
	if (styleset != this->styleset)
	{
		//somebody just changed the styleset!
		GUIentity::SetStyleset(styleset);
		auto panelit = panels.find(curPanelId);
		if (panelit != panels.end())
		{
			panels[curPanelId]->redrawPanel(this, GetStyle("default"));
		}
	}
}

MESHHANDLE GUImanager::GetPanelMesh()
{
	return hPanelMesh;
}
