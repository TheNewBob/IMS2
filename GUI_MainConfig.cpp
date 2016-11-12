#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUI_MainConstruct.h"
#include "GUIentity.h"
#include "GUIplugin.h"
#include "GUImanager.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "GUI_Module.h"
#include "IMS_InputCallback.h"
#include "GUI_MainConfig.h"
#include "Managers.h"
#include "IMS_RcsManager.h"

GUI_MainConfig::GUI_MainConfig(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_CONFIG_MENU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();

	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	rcs_assist = gui->CreateCheckBox("RCS Assist", _R(10, 10, width / 2 - 10, 35), id, MD_CONFIG_RCSASSISTCHKBX);
	rcs_assist->SetChecked(true);
}



GUI_MainConfig::~GUI_MainConfig()
{
}



int GUI_MainConfig::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//get the clicked element
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	if (eventId == -1) return -1;

	//handle results of click events
	switch (eventId)
	{
	case  MD_CONFIG_RCSASSISTCHKBX:
		vessel->GetRcsManager()->SetIntelligentRcs(rcs_assist->Checked());
		break;
	}

	return eventId;
}
