#include "Common.h"
#include "IMS.h"
#include "GUI_Common.h"
#include "GUI_BaseElement.h"
#include "GUI_Page.h"
#include "GUI_Surface_Base.h"
#include "GUI_Surface.h"
#include "IMS2_GUI_Surface.h"


IMS2_GUI_Surface::IMS2_GUI_Surface(IMS2 *vessel, int panelid, GUI_Page *rootelement)
	: GUI_Surface(vessel, panelid, vessel->GetGUI(), rootelement)
{
}


IMS2_GUI_Surface::~IMS2_GUI_Surface()
{
}
