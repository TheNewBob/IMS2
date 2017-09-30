#include "Common.h"
#include "Events.h"
#include "IMS.h"
#include "GUI_Common.h"
#include "GUI_Panel.h"
#include "IMS_EngineeringPanel.h"
#include "GUI_elements.h"
#include "IMS_RootPage.h"
#include "GUI_MainDisplay.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"
#include "GUIentity.h"
#include "GUImanager.h"

SURFHANDLE IMS_EngineeringPanel::surfhandle = NULL;

IMS_EngineeringPanel::IMS_EngineeringPanel(float maxWidthEm, float minWidthEm, float maxHeightEm, float minHeightEm,
	int viewPortWidthPx, int viewPortHeightPx, IMS2 *vessel, GUImanager *gui)
	: GUI_Panel(maxWidthEm, minWidthEm, maxHeightEm, minHeightEm, viewPortWidthPx, viewPortHeightPx, 1)
{
	surfhandle = oapiCreateSurfaceEx(width, height, OAPISURFACE_TEXTURE);
	GUI_Panel::setSurfaceHandle(&surfhandle);

	//create SwingShot panel elements
	RECT maindisplayrect = _R(width * 0.7, height * 0.4, width * 0.997, height * 0.997);
	//RECT maindisplayrect = _R(1176, 428, 1676, 1046);
	GUI_MainDisplay *maindisplay = new GUI_MainDisplay(vessel, _R(0, 0, maindisplayrect.right - maindisplayrect.left, maindisplayrect.bottom - maindisplayrect.top), gui->GetStyle(STYLE_PAGE));
	maindisplay_surf = new GUI_Surface(vessel, (int)ENGINEERINGPANEL, gui, maindisplay);
	gui->RegisterGUISurface(maindisplay_surf, GUI_MAIN_DISPLAY, maindisplayrect);
}


IMS_EngineeringPanel::~IMS_EngineeringPanel()
{
}

DWORD IMS_EngineeringPanel::registerPanelElements(GUImanager *gui, PANELHANDLE panelHandle, VESSEL3 *vessel)
{
	IMS2 *imsvessel = dynamic_cast<IMS2*>(vessel);
	assert(imsvessel != NULL && "IMS_EngineeringPanel requires an IMS2 vessel!");

	oapiSetPanelNeighbours(-1, -1, 0, -1); // register areas for panel 0 here 
	vessel->SetCameraDefaultDirection(_V(0, 0, 1)); // forward
	oapiCameraSetCockpitDir(0, 0);         // look forward

	//register GUI_Surface with Orbiter
	gui->RegisterSurfaceWithOrbiter(maindisplay_surf, panelHandle, surfhandle);
	// TODO: method has changed object. Check if this still needs to be executed.
	//mainDispSurface->UpdateDockedVesselsList(dockedVesselsList);

	return PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM | PANEL_MOVEOUT_TOP;
}
