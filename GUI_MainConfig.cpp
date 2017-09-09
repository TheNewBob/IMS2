#include "GUI_Common.h"
#include "Common.h"
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
#include "IMS_TouchdownPointManager.h"
#include "LayoutManager.h"
#include "GUI_Looks.h"

const static string FILENAME = "mainmenu/config.xml";
const static string RCS_ASSIST_CHKBX = "rcs_assist_chkbx";
const static string SCENED_ASSIST_CHKBX = "scened_assist_chkbx";
const static string STYLESETS_LISTBX = "stylesets_listbx";
const static string STYLESETS_LBL = "stylesets_lbl";

GUI_MainConfig::GUI_MainConfig(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_CONFIG_MENU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();
	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);

	rcs_assist = gui->CreateCheckBox("RCS Assist", getElementRect(RCS_ASSIST_CHKBX, layouts), id, MD_CONFIG_RCSASSISTCHKBX, getElementStyle(RCS_ASSIST_CHKBX, layouts));
	rcs_assist->SetChecked(true);

	scened_assist = gui->CreateCheckBox("ScenEd placement assist", getElementRect(SCENED_ASSIST_CHKBX, layouts), id, MD_CONFIG_SCENEDCHKBX, getElementStyle(SCENED_ASSIST_CHKBX, layouts));
	scened_assist->SetChecked(false);

	gui->CreateLabel("Select Skin", getElementRect(STYLESETS_LBL, layouts), id, -1, getElementStyle(STYLESETS_LBL, layouts));
	stylesets = gui->CreateListBox(getElementRect(STYLESETS_LISTBX, layouts), id, MD_CONFIG_STYLESETSLISTBX, getElementStyle(STYLESETS_LISTBX, layouts));
	
	//populate the listbox with the available styles
	vector<string> availablestylesets = GUI_Looks::GetAvailableStyleSets();
	string currentstyleset = gui->GetStyleSet();

	for (UINT i = 0; i < availablestylesets.size(); ++i)
	{
		string addedstyleset = availablestylesets[i];
		if (currentstyleset == addedstyleset)
		{
			stylesets->AddElement(addedstyleset, true);
		}
		else
		{
			stylesets->AddElement(addedstyleset);
		}
	}
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
		//toggle intelligent RCS assistance	
		vessel->GetRcsManager()->SetIntelligentRcs(rcs_assist->Checked());
		break;
	case MD_CONFIG_SCENEDCHKBX:
		//toggle scenario editor placement assistance
		vessel->GetTdPointManager()->SetScenedAssistance(scened_assist->Checked());
		break;

	case MD_CONFIG_STYLESETSLISTBX:
		//set a new styleset
		vessel->GetGUI()->SetStyleSet(stylesets->GetSelectedText());
		break;
	}

	return eventId;
}
