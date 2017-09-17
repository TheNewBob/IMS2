#include "GUI_Common.h"
#include "GUI_BaseElement.h"
#include "GUI_Page.h"
#include "GUI_Surface_Base.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUI_Surface.h"



GUI_Surface::GUI_Surface(VESSEL3 *vessel, int panelid, GUImanager *gui, GUI_Page *rootelement)
	: GUI_Surface_Base(vessel, panelid, gui), page(rootelement)
{
}

GUI_Surface::~GUI_Surface()
{
}

void GUI_Surface::PostInit()
{
	assert(isInitialised);
	gui->RegisterGuiRootPage(page);
	// let the page create its children
	page->PostConstruction(gui);
}

bool GUI_Surface::Redraw2D(SURFHANDLE surf)
{
	assert(isInitialised);
	page->Draw(surf, rect, rect.left, rect.top);
	return true;
}


bool GUI_Surface::ProcessMouse2D (GUI_MOUSE_EVENT _event, int mx, int my)
{
	assert(isInitialised);
	int clickedElement = page->ProcessMouse(_event, mx, my);
	if (clickedElement != -1)
	{
		gui->RedrawGUISurface(id);
		return true;
	}
	return false;
}

bool GUI_Surface::Update()
{
	assert(isInitialised);
	if (page->Update())
	{
		gui->RedrawGUISurface(id);
		return true;
	}
	return false;
}

