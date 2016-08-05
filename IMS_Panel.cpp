//#include "Common.h"
#include "GUI_Common.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "Events.h"
#include "IMS.h"
#include "GUI_Surface.h"
#include "GUI_MainDisplay.h"

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
			break; 

		case ENGINEERINGPANEL: 
			DefineEngPanel(hPanel);
			SetPanelScale (hPanel, viewW, viewH);
			oapiSetPanelNeighbours (-1, -1, 0, -1); // register areas for panel 1 here 
			SetCameraDefaultDirection (_V(0,0,1)); // forward
			oapiCameraSetCockpitDir (0,0);         // look forward
			break;
	}

	GUI->SetCurPanelId(id);
	GUI->RedrawCurrentPanel();
	return true;
}


void IMS2::DefineMainPanel(PANELHANDLE hPanel)
{
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

void IMS2::DefineEngPanel(PANELHANDLE hPanel)
{
	  static DWORD panelW = 1680;
	  static DWORD panelH = 1050;
	  float fpanelW = (float)panelW;
	  float fpanelH = (float)panelH;
	  static DWORD texW   = 1680;
	  static DWORD texH   = 1050;
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

	  if (hPanelMesh) oapiDeleteMesh (hPanelMesh);
	  hPanelMesh = oapiCreateMesh (0,0);
	  MESHGROUP grp = {VTX, IDX, 4, 6, 0, 0, 0, 0, 0};
	  oapiAddMeshGroup (hPanelMesh, &grp);

	  SetPanelBackground (hPanel, &engPanelBG, 1, hPanelMesh, panelW, panelH, 0,
	    PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM | PANEL_MOVEOUT_TOP);

	//register GUI_Surface with Orbiter
	  GUI->RegisterSurfaceWithOrbiter(mainDispSurface, hPanel, engPanelBG);
	  mainDispSurface->UpdateDockedVesselsList(dockedVesselsList);
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
	hPanelMesh = NULL;
	GUI = new GUImanager(this);

	
	//create panel backgrounds if they are not yet initialised
	if (engPanelBG == NULL)
	{
		engPanelBG = oapiCreateSurfaceEx(1680, 1050, OAPISURFACE_TEXTURE /*| OAPISURFACE_RENDERTARGET | OAPISURFACE_GDI | OAPISURFACE_SKETCHPAD*/);
//		engPanelBG = oapiCreateTextureSurface(1680, 1050);

		GUI_ElementStyle *defaultstyle = GUI->GetStyle();
		oapiColourFill(engPanelBG, oapiGetColour(defaultstyle->BackgroundColor().r,
			defaultstyle->BackgroundColor().g,
			defaultstyle->BackgroundColor().b), 0, 0, 1680, 1050);
	}
	if (pilotPanelBG == NULL)
	{
		pilotPanelBG = GUI_Draw::createElementBackground(GUI->GetStyle(), 1680, 1050);
	}
	//allocate panel elements
	mainDispSurface = new GUI_MainDisplay(this, GUI->GetStyle(STYLE_WINDOW_BORDER), ENGINEERINGPANEL);
	GUI->RegisterGUISurface(mainDispSurface, GUI_MAIN_DISPLAY, _R(1176, 428, 1676, 1046));
}

void IMS2::DestroyGUI()
{
	//debug
	string vesName = GetName();

	if (hPanelMesh) oapiDeleteMesh (hPanelMesh);
	delete GUI;
	if (mainDispSurface != NULL)
	//if noone actually looked at the panel, the GUI surfaces will never get registered and therefore not deleted
	//if that is the case, delete them here
	{
		delete mainDispSurface;
	}
}

void IMS2::updateGui()
{
	GUI->Update();
}


