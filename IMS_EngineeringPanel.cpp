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
#include "LayoutManager.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"

SURFHANDLE IMS_EngineeringPanel::surfhandle = NULL;

const string FILENAME = "engineeringpanel.xml";
const string MAINDISPLAY = "main_display";

IMS_EngineeringPanel::IMS_EngineeringPanel(float maxWidthEm, float minWidthEm, 
	int viewPortWidthPx, int viewPortHeightPx, IMS2 *vessel, GUImanager *gui)
	: GUI_Panel(GUI_Layout::EmToPx(maxWidthEm), GUI_Layout::EmToPx(minWidthEm), 0, viewPortWidthPx, viewPortHeightPx, 1)
{
	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);
	GUI_Layout *activelayout = layouts->GetLayoutForWidth(width);
	height = activelayout->GetLayoutHeight();
	
	surfhandle = oapiCreateSurfaceEx(width, height, OAPISURFACE_TEXTURE);
	GUI_Panel::setSurfaceHandle(&surfhandle);

	//create panel elements
	LAYOUTDATA layoutdata = activelayout->GetLayoutDataForField(MAINDISPLAY, width);
	RECT relativerect = _R(0, 0, layoutdata.rect.right - layoutdata.rect.left, layoutdata.rect.bottom - layoutdata.rect.top);   //the rect of maindisplay will be relative to its parent, the surface, not the panel.
	string styleid = layoutdata.styleId == "" ? STYLE_PAGE : layoutdata.styleId;		//use page as default style if none defined in layout.
	GUI_MainDisplay *maindisplay = new GUI_MainDisplay(vessel, relativerect, gui->GetStyle(styleid));

	maindisplay_surf = new GUI_Surface(vessel, (int)ENGINEERINGPANEL, gui, maindisplay);
	gui->RegisterGUISurface(maindisplay_surf, GUI_MAIN_DISPLAY, layoutdata.rect);
}


IMS_EngineeringPanel::~IMS_EngineeringPanel()
{
}

DWORD IMS_EngineeringPanel::registerPanelElements(GUImanager *gui, PANELHANDLE panelHandle, VESSEL3 *vessel)
{
	IMS2 *imsvessel = dynamic_cast<IMS2*>(vessel);
	Helpers::assertThat([imsvessel]() { return imsvessel != NULL; }, "IMS_EngineeringPanel requires an IMS2 vessel!");

	oapiSetPanelNeighbours(-1, -1, 0, -1); // register areas for panel 0 here 
	vessel->SetCameraDefaultDirection(_V(0, 0, 1)); // forward
	oapiCameraSetCockpitDir(0, 0);         // look forward

	//register GUI_Surface with Orbiter
	gui->RegisterSurfaceWithOrbiter(maindisplay_surf, panelHandle, surfhandle);
	// TODO: method has changed object. Check if this still needs to be executed.
	//mainDispSurface->UpdateDockedVesselsList(dockedVesselsList);

	return PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM | PANEL_MOVEOUT_TOP;
}
