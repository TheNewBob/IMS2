#include "GUI_Common.h"
#include "Events.h"
#include "GUI_Surface.h"
#include "IMS.h"
#include "GUIentity.h"
#include "GUImanager.h"




GUI_Surface::GUI_Surface(IMS2 *_vessel, GUI_ElementStyle *_style, int _panelid)
	: PanelElement(_vessel), vessel(_vessel), style(_style), isInitialised(false)
{
	//create and connect the event sink to receive vessel events
	EventSink *temp = createEventSink(VESSEL_TO_GUI_PIPE);
	vessel->ConnectToMyEventGenerator(temp, VESSEL_TO_GUI_PIPE);
	panelId = _panelid;
	eventhandlertype = GUISURFACE_HANDLER;
}

GUI_Surface::~GUI_Surface()
{

}

void GUI_Surface::Init(RECT &surfacePos, int _id) 
{
	rect = surfacePos;
	height = rect.bottom - rect.top;
	width = rect.right - rect.left;
	id = _id;
	createGuiSurface();
	isInitialised = true;
}


bool GUI_Surface::Redraw2D(SURFHANDLE surf)
{
	assert(isInitialised);
	RedrawBackground(surf);
	RedrawElements(surf);
	return true;
}


bool GUI_Surface::ProcessMouse2D (GUI_MOUSE_EVENT _event, int mx, int my)
{
	assert(isInitialised);
	int triggeredId = ProcessChildren(_event, mx, my);
	if (ProcessMe(triggeredId))
	{
		vessel->GetGUI()->RedrawGUISurface(id);
		return true;
	}
	return false;
}

bool GUI_Surface::Update()
{
	assert(isInitialised);
	bool needsupdate = false;
	//check if any child calls for an update
	for (UINT i = 0; i < children.size(); ++i)
	{
		if (children[i]->Update())
		{
			needsupdate = true;
		}
	}
	if (needsupdate)
	{
		vessel->GetGUI()->RedrawGUISurface(id);
	}
	return needsupdate;
}

bool GUI_Surface::ProcessMe(int eventId)
//virtual function. Use to implement eventhandling in individual GUI surfaces
{
	assert(isInitialised);
	if (eventId != -1)
	{
		vessel->GetGUI()->RedrawGUISurface(id);
		return true;
	}
	return false;
}

void GUI_Surface::RedrawBackground(SURFHANDLE surf)
{
	assert(isInitialised);
	oapiBlt(surf, src, rect.left, rect.top, 0, 0, width, height);
	//	oapiBlt(surf, src, &rect, &srcRect);
}

void GUI_Surface::RedrawElements(SURFHANDLE surf)
{
	assert(isInitialised);
	RECT drawablerect = _R(rect.left + style->MarginLeft(),
							rect.top + style->MarginTop(),
							rect.right - style->MarginRight(),
							rect.bottom - style->MarginBottom());

	for (UINT i = 0; i < children.size(); ++i)
	{
		children[i]->Draw(surf, drawablerect, rect.left, rect.top);
	}
}


//creates the actual drawing surface
void GUI_Surface::createGuiSurface()
{
	SURFHANDLE tgt = GUI_Draw::createElementBackground(style, width, height);
	Sketchpad *skp = oapiGetSketchpad(tgt);
	GUI_Draw::DrawRectangle(_R(0, 0, width, height), skp, style);
	oapiReleaseSketchpad(skp);
	src = tgt;
}

int GUI_Surface::GetId()
{
	assert(isInitialised);
	return id;
}

int GUI_Surface::GetPanelId()
{
	assert(isInitialised);
	return panelId;
}

void GUI_Surface::AddChild(GUI_BaseElement *child)
{
	assert(isInitialised);
	children.push_back(child);
}



int GUI_Surface::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	assert(isInitialised);
	int triggeredElement = -1;
	for (UINT i = 0; i < children.size(); ++i)
	{
		triggeredElement = children[i]->ProcessMouse(_event, _x, _y);
		if (triggeredElement != -1)
		{
			break;
		}
	}
	return triggeredElement;
}

