#include "orbitersdk.h"
#include "GUI_Surface_Base.h"


GUI_Surface_Base::GUI_Surface_Base(VESSEL3 *vessel, int panelid)
	: vessel(vessel), panelId(panelid)
{
}


GUI_Surface_Base::~GUI_Surface_Base()
{
}

void GUI_Surface_Base::Init(RECT &surfacePos, int id)
{
	rect = surfacePos;
	height = rect.bottom - rect.top;
	width = rect.right - rect.left;
	this->id = id;
	isInitialised = true;
}

bool GUI_Surface_Base::ProcessMouse2D(int event, int mx, int my){
	return false;
}
