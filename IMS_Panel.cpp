#include "Common.h"
#include "GUI_Common.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "Events.h"
#include "IMS.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"
#include "GUI_BaseElement.h"
#include "GUI_Page.h"
#include "IMS_RootPage.h"
#include "GUI_MainDisplay.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"
#include "GUI_Panel.h"
#include "IMS_EngineeringPanel.h"

bool IMS2::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) 
{
//	ReleaseSurfaces();

	switch (id) { 

		case FLIGHTPANEL: 
			DefineMainPanel (hPanel);
			SetPanelScale (hPanel, viewW, viewH);
			oapiSetPanelNeighbours (-1, -1, -1, 1); // register areas for panel 0 here 
			SetCameraDefaultDirection (_V(0,0,1)); // forward
			oapiCameraSetCockpitDir (0,0);         // look forward
			//redraw areas
			GUI->SetCurPanelId(id);
			GUI->RedrawCurrentPanel();
			break;

		default:
			GUI->LoadPanel(id, hPanel);
			break;
	}

	return true;
}


void IMS2::DefineMainPanel(PANELHANDLE hPanel)
{
	static MESHHANDLE hPanelMesh = NULL;
	  static DWORD panelW = 1280;
	  static DWORD panelH =  800;
	  float fpanelW = (float)panelW;
	  float fpanelH = (float)panelH;
	  static DWORD texW   = 2048;
	  static DWORD texH   =  1024;
	  float ftexW   = (float)texW;
	  float ftexH   = (float)texH;

	  static NTVERTEX VTX[4] = {
		{      0,      0,0,   0,0,0,   0.0f,1.0f-fpanelH/ftexH},
	    {      0,fpanelH,0,   0,0,0,   0.0f,1.0f},
		{fpanelW,fpanelH,0,   0,0,0,   fpanelW/ftexW,1.0f},
	    {fpanelW,      0,0,   0,0,0,   fpanelW/ftexW,1.0f-fpanelH/ftexH}
	  };

	  static WORD IDX[6] = {
	    0,2,1,
		2,0,3
	  };

	  if (hPanelMesh) oapiDeleteMesh (hPanelMesh);
	  hPanelMesh = oapiCreateMesh (0,0);
	  MESHGROUP grp = {VTX, IDX, 4, 6, 0, 0, 0, 0, 0};
	  oapiAddMeshGroup (hPanelMesh, &grp);

	  SetPanelBackground (hPanel, &pilotPanelBG, 1, hPanelMesh, panelW, panelH, 0,
	    PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM | PANEL_MOVEOUT_TOP);
	
}

void IMS2::DefineEngPanel(PANELHANDLE hPanel, DWORD width, DWORD height)
{
	  float fpanelW = (float)width;
	  float fpanelH = (float)height;
	  static DWORD texW   = width;
	  static DWORD texH   = height;
	  float ftexW   = (float)texW;
	  float ftexH   = (float)texH;

	  static NTVERTEX VTX[4] = {
		{      0,      0,0,   0,0,0,   0.0f,0.0f},
	    {      0,fpanelH,0,   0,0,0,   0.0f,fpanelH/ftexH},
		{fpanelW,fpanelH,0,   0,0,0,   fpanelW/ftexW,fpanelH/ftexH},
	    {fpanelW,      0,0,   0,0,0,   fpanelW/ftexW,0.0f}
	  };

	  static WORD IDX[6] = {
	    0,2,1,
		2,0,3
	  };

/*	  if (hPanelMesh) oapiDeleteMesh (hPanelMesh);
	  hPanelMesh = oapiCreateMesh (0,0);
	  MESHGROUP grp = {VTX, IDX, 4, 6, 0, 0, 0, 0, 0};
	  oapiAddMeshGroup (hPanelMesh, &grp);

	  SetPanelBackground (hPanel, &engPanelBG, 1, hPanelMesh, width, height, 0,
	    PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM | PANEL_MOVEOUT_TOP);

	//register GUI_Surface with Orbiter
	  GUI->RegisterSurfaceWithOrbiter(mainDispSurface, hPanel, engPanelBG);
	  // TODO: method has changed object. Check if this still needs to be executed.
	  //mainDispSurface->UpdateDockedVesselsList(dockedVesselsList);*/
}



void IMS2::SetPanelScale (PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	double defscale = (double)viewW/1680;
	double extscale = max (defscale, 1.0);
	SetPanelScaling (hPanel, defscale, extscale);
}




bool IMS2::clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf, void *context)
{
	if (context) 
	{
		GUI_Surface* cursurf = (GUI_Surface*)context;
		cursurf->Redraw2D(surf);
		return true;
	}
	return false;
}





bool IMS2::clbkPanelMouseEvent (int id, int event, int mx, int my, void *context)
{
	if (oapiGetFocusInterface() != this) return false;		//if element is triggered while this vessel is not focused, ignore

	if (context) 
	{
		GUI_Surface *cursurf = (GUI_Surface*)context;
	    return cursurf->ProcessMouse2D (GUI->GetEvent(event), mx, my);
	} 

	return false;
}


void IMS2::InitialiseGUI()
{
	//initalising GUI for vessel
	// hPanelMesh = NULL;
	GUI = new GUImanager(this);
	DWORD screenWidth, screenHeight, depth;
	oapiGetViewportSize(&screenWidth, &screenHeight, &depth);
	
	//create panel backgrounds if they are not yet initialised
	IMS_EngineeringPanel *engpanel = new IMS_EngineeringPanel(120, 64, screenWidth, screenHeight, this, GUI);
	GUI->RegisterGUIPanel(engpanel);
	/*	if (engPanelBG == NULL)
	{
		engPanelBG = oapiCreateSurfaceEx(screenWidth, screenHeight, OAPISURFACE_TEXTURE);

		GUI_ElementStyle *defaultstyle = GUI->GetStyle("default");
		oapiColourFill(engPanelBG, oapiGetColour(defaultstyle->FillColor().r,
			defaultstyle->FillColor().g,
			defaultstyle->FillColor().b), 0, 0, screenWidth, screenHeight);
	}*/
	if (pilotPanelBG == NULL)
	{
		pilotPanelBG = oapiCreateSurfaceEx(1680, 1050, OAPISURFACE_TEXTURE);

		GUI_ElementStyle *defaultstyle = GUI->GetStyle("default");
		oapiColourFill(pilotPanelBG, oapiGetColour(defaultstyle->FillColor().r,
			defaultstyle->FillColor().g,
			defaultstyle->FillColor().b), 0, 0, 1680, 1050);
	}

	//create SwingShot panel elements
/*	RECT maindisplayrect = _R(screenWidth * 0.7, screenHeight * 0.4, screenWidth * 0.997, screenHeight * 0.997);
	//RECT maindisplayrect = _R(1176, 428, 1676, 1046);
	GUI_MainDisplay *maindisplay = new GUI_MainDisplay(this, _R(0, 0, maindisplayrect.right - maindisplayrect.left, maindisplayrect.bottom - maindisplayrect.top), GUI->GetStyle(STYLE_PAGE));
	mainDispSurface = new GUI_Surface(this, ENGINEERINGPANEL, GUI, maindisplay);
	GUI->RegisterGUISurface(mainDispSurface, GUI_MAIN_DISPLAY, maindisplayrect);*/
}

void IMS2::DestroyGUI()
{
//	if (hPanelMesh) oapiDeleteMesh (hPanelMesh);
	delete GUI;
}

void IMS2::updateGui()
{
	GUI->Update();
}

void IMS2::initialiseStyleset()
{
	GUI->SetStyleset(loadedstyleset);
}
