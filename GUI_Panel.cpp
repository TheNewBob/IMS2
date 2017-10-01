#include "GUI_Common.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "GUI_Panel.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"

GUI_ElementStyle *GUI_Panel::style = NULL;


GUI_Panel::GUI_Panel(float maxWidthPx, float minWidthPx, float heightPx, 
	int viewPortWidthPx, int viewPortHeightPx, int panelId) : id(panelId)
{
	// calculate the panels rect in pixel
	width = max(min(maxWidthPx, viewPortWidthPx), minWidthPx);
	height = heightPx;
}


GUI_Panel::~GUI_Panel()
{
}

void GUI_Panel::redrawPanel(GUImanager *gui, GUI_ElementStyle *style) 
{
	this->style = style;
	oapiColourFill(*srf, oapiGetColour(style->FillColor().r,
		style->FillColor().g,
		style->FillColor().b), 
		0, 0, width, height);

	gui->RedrawCurrentPanel();
}


bool GUI_Panel::loadPanel(GUImanager *gui, PANELHANDLE panelHandle,
						  MESHHANDLE hPanelMesh, GUI_ElementStyle *style, VESSEL3 *vessel)
{
	if (this->style != style)
	{
		//The style has changed, redraw the background.
		redrawPanel(gui, style);
	}
	
	vessel->SetPanelScaling(panelHandle, 1.0, 1.0);

	
	float fpanelW = (float)width;
	float fpanelH = (float)height;

	static NTVERTEX VTX[4] = {
		{ 0, 0, 0, 0, 0, 0, 0.0f, 0.0f },
		{ 0, fpanelH, 0, 0, 0, 0, 0.0f, 1.0f },
		{ fpanelW, fpanelH, 0, 0, 0, 0, 1.0f, 1.0f },
		{ fpanelW, 0, 0, 0, 0, 0, 1.0f, 0.0f }
	};

	static WORD IDX[6] = {
		0, 2, 1,
		2, 0, 3
	};

	if (hPanelMesh) oapiDeleteMesh(hPanelMesh);
	hPanelMesh = oapiCreateMesh(0, 0);
	MESHGROUP grp = { VTX, IDX, 4, 6, 0, 0, 0, 0, 0 };
	oapiAddMeshGroup(hPanelMesh, &grp);

	DWORD scrollflags = registerPanelElements(gui, panelHandle, vessel);

	vessel->SetPanelBackground(panelHandle, srf, 1, hPanelMesh, width, height, 0, scrollflags);
	return true;
}

void GUI_Panel::setSurfaceHandle(SURFHANDLE *surfhandle)
{
	srf = surfhandle;
}

