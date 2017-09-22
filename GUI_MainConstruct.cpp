#include "Common.h"
#include "GUI_Common.h"
#include "GUI_Elements.h"
#include "GUI_MainConstruct.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "IMS_InputCallback.h"
#include "LayoutManager.h"

/**
* \file defines content and functionality for the construction menu.
*/

const static string FILENAME				= "mainmenu/construct.xml";
const static string DOCKED_VESSELS_LABEL	= "docked_vessels_label";
const static string DOCKED_VESSELS_LIST		= "docked_vessels_list";
const static string INTEGRATE_BTNS_PAGE		= "integrate_btns_page";
const static string ROTATE_DOCKED_BTN		= "rotate_docked_btn";
const static string INTEGRATE_DOCKED_BTN	= "integrate_docked_btn";
const static string DETACH_DOCKED_BTN		= "detach_docked_btn";
const static string IN_STACK_LABEL			= "in_stack_label";
const static string IN_STACK_LIST			= "in_stack_list";
const static string UPDATE_STACK_BTN		= "update_stack_btn";
const static string ASSEMBLE_ALL_BTN		= "assemble_all_btn";

GUI_MainConstruct::GUI_MainConstruct(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_CONSTRUCTION_MNU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();
	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);

	gui->CreateLabel(getLayoutDataForElement(DOCKED_VESSELS_LABEL, layouts), "docked vessels", MD_CONSTRUCTION_MNU, -1);
	dockedVesselsList = gui->CreateListBox(getLayoutDataForElement(DOCKED_VESSELS_LIST, layouts), MD_CONSTRUCTION_MNU, MD_CM_DOCKED_VESSELS_LIST);

	integrateButtonsPage = gui->CreatePage(getLayoutDataForElement(INTEGRATE_BTNS_PAGE, layouts), MD_CONSTRUCTION_MNU, MD_CM_INT_BTNS_PAGE);
	integrateButtonsPage->SetVisible(false);

	gui->CreateDynamicButton(getLayoutDataForElement(ROTATE_DOCKED_BTN, layouts), "rotate", MD_CM_INT_BTNS_PAGE, MD_CM_ROTATE_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(INTEGRATE_DOCKED_BTN, layouts), "integrate", MD_CM_INT_BTNS_PAGE, MD_CM_INTEGRATE_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(DETACH_DOCKED_BTN, layouts), "detach", MD_CM_INT_BTNS_PAGE, MD_CM_DETATCH_BTN);

	gui->CreateLabel(getLayoutDataForElement(IN_STACK_LABEL, layouts), "vessels in stack", MD_CONSTRUCTION_MNU, -1);
	stackVesselsList = gui->CreateListBox(getLayoutDataForElement(IN_STACK_LIST, layouts), MD_CONSTRUCTION_MNU, MD_CM_STACKED_VESSELS_LIST);

	gui->CreateDynamicButton(getLayoutDataForElement(UPDATE_STACK_BTN, layouts), "update stack", MD_CONSTRUCTION_MNU, MD_CM_UPDATE_STACK_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(ASSEMBLE_ALL_BTN, layouts), "assemble all", MD_CONSTRUCTION_MNU, MD_CM_ASSEMBLE_ALL_BTN);
}


GUI_MainConstruct::~GUI_MainConstruct()
{
}



int GUI_MainConstruct::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//get the clicked element
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	if (eventId == -1) return -1;

	//handle results of click events
	switch (eventId)
	{
	case MD_CM_DOCKED_VESSELS_LIST:
		if (dockedVesselsList->GetSelected() != -1)
		{
			integrateButtonsPage->SetVisible(true);
		}
		else
		{
			integrateButtonsPage->SetVisible(false);
		}
		break;

	case MD_CM_INTEGRATE_BTN:
		if (dockedVesselsList->GetSelected() != -1)
		{
			vector<string> ves;
			ves.push_back(dockedVesselsList->GetSelectedText());
			vessel->AssimilateIMSVessels(ves);
		}
		break;
	case MD_CM_ROTATE_BTN:
		if (dockedVesselsList->GetSelected() != -1)
		{
			OBJHANDLE dvesH = oapiGetVesselByName((char*)(dockedVesselsList->GetSelectedText().data()));
			IMSATTACHMENTPOINT *att = vessel->GetConnectingAttachmentPoint(oapiGetVesselInterface(dvesH));
			oapiOpenInputBox("enter degrees", RotateAttachmentPointCallback, 0, 15, att);
		}
		break;
	case MD_CM_UPDATE_STACK_BTN:
	{
		vector<VESSEL*> stackVessels;
		stackVesselsList->clear();
		vessel->GetStack(stackVessels);
		for (UINT i = 0; i < stackVessels.size(); ++i)
		{
			stackVesselsList->AddElement(stackVessels[i]->GetName());
		}
		break;
	}
	case MD_CM_ASSEMBLE_ALL_BTN:
		vector<VESSEL*> stackVessels;
		//clearing the listbox, since afterwards there won't be any stacked vessels
		stackVesselsList->clear();
		//get the stack anew. Much saver than using the potentially out of date listbox
		vessel->GetStack(stackVessels);
		vector<string> sVesselNameList;
		for (UINT i = 0; i < stackVessels.size(); ++i)
			//creating a vector with only the vessel names
		{
			sVesselNameList.push_back(stackVessels[i]->GetName());
		}
		//assimilate all vessels in list
		vessel->AssimilateIMSVessels(sVesselNameList);
		break;
	}
	return eventId;
}

void GUI_MainConstruct::UpdateDockedVesselsList(std::vector<DOCKEDVESSEL*> &vesselList)
{
	dockedVesselsList->clear();
	for (UINT i = 0; i < vesselList.size(); ++i)
	{
		if (vesselList[i]->type == IMS_VESSEL)
		{
			string newEntry = vesselList[i]->vessel->GetName();
			dockedVesselsList->AddElement(newEntry);
		}
	}
	if (dockedVesselsList->GetSelected() < 0)
	{
		integrateButtonsPage->SetVisible(false);
	}
}

