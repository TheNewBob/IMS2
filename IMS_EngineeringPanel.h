#pragma once
class GUI_Surface;
class GUImanager;
class IMS2;

class IMS_EngineeringPanel :
	public GUI_Panel
{
public:
	IMS_EngineeringPanel(float maxWidthEm, float minWidthEm, float maxHeightEm, float minHeightEm,
						 int viewPortWidthPx, int viewPortHeightPx, IMS2 *vessel, GUImanager *gui);
	~IMS_EngineeringPanel();


private:
	static SURFHANDLE surfhandle;
	DWORD registerPanelElements(GUImanager *gui, PANELHANDLE panelHandle, VESSEL3 *vessel);
	GUI_Surface *maindisplay_surf;
};

