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

GUI_MainConstruct::GUI_MainConstruct(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_LayoutPage(mRect, MD_CONSTRUCTION_MNU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();
	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	layouts = LayoutManager::GetLayout("");

/*	GUI_Layout *testlayout = new GUI_Layout;
	vector<LayoutField> row;
	LayoutField field;
	field.width = 1.0;
	field.elementid = "title";
	row.push_back(field);
	testlayout->AddRow(row, 25);
	field.width = 0.6;
	field.elementid = "dockedVesselsList";
	row.clear();
	row.push_back(field);
	field.width = 0.4;
	field.elementid = "integrateButtonsPage";
	row.push_back(field);
	testlayout->AddRow(row, 250);

	layouts = new LAYOUTCOLLECTION;
	layouts->AddLayout(1, testlayout);*/

	gui->CreateLabel("docked vessels", getElementRect("docked_vessels_label"), MD_CONSTRUCTION_MNU);
	dockedVesselsList = gui->CreateListBox(getElementRect("docked_vessels_list"), MD_CONSTRUCTION_MNU, MD_CM_DOCKED_VESSELS_LIST);

	integrateButtonsPage = gui->CreatePage(getElementRect("integrate_buttons_page"), MD_CONSTRUCTION_MNU, MD_CM_INT_BTNS_PAGE);
	integrateButtonsPage->SetVisible(false);

	gui->CreateDynamicButton("rotate", getElementRect("rotate_docked_btn"), MD_CM_INT_BTNS_PAGE, MD_CM_ROTATE_BTN);
	gui->CreateDynamicButton("integrate", getElementRect("integrate_docked_btn"), MD_CM_INT_BTNS_PAGE, MD_CM_INTEGRATE_BTN);
	gui->CreateDynamicButton("detach", getElementRect("detach_docked_btn"), MD_CM_INT_BTNS_PAGE, MD_CM_DETATCH_BTN);

	gui->CreateLabel("vessels in stack", getElementRect("in_stack_label"), MD_CONSTRUCTION_MNU);
	stackVesselsList = gui->CreateListBox(getElementRect("stacked_vessels_list"), MD_CONSTRUCTION_MNU, MD_CM_STACKED_VESSELS_LIST);

	gui->CreateDynamicButton("update stack", getElementRect("update_stack_btn"), MD_CONSTRUCTION_MNU, MD_CM_UPDATE_STACK_BTN);
	gui->CreateDynamicButton("assemble all", getElementRect("assemble_all_btn"), MD_CONSTRUCTION_MNU, MD_CM_ASSEMBLE_ALL_BTN);

	/*
	//docked vessels operations
	gui->CreateLabel("docked vessels", _R(30, 0, int(width * 0.6), 25), MD_CONSTRUCTION_MNU);
	dockedVesselsList = gui->CreateListBox(_R(30, 30, int(width * 0.6), 280), MD_CONSTRUCTION_MNU, MD_CM_DOCKED_VESSELS_LIST);

	integrateButtonsPage = gui->CreatePage(_R(int(width * 0.6) + 10, 0, 450, 225), MD_CONSTRUCTION_MNU, MD_CM_INT_BTNS_PAGE);
	integrateButtonsPage->SetVisible(false);

	gui->CreateDynamicButton("rotate", _R(style->MarginLeft(), 30, 120, 55), MD_CM_INT_BTNS_PAGE, MD_CM_ROTATE_BTN);
	gui->CreateDynamicButton("integrate", _R(style->MarginLeft(), 60, 120, 85), MD_CM_INT_BTNS_PAGE, MD_CM_INTEGRATE_BTN);
	gui->CreateDynamicButton("detach", _R(style->MarginLeft(), 90, 120, 115), MD_CM_INT_BTNS_PAGE, MD_CM_DETATCH_BTN);
	

	//stacked vessels operations
	gui->CreateLabel("vessels in stack", _R(30, 290, int(width * 0.6), 315), MD_CONSTRUCTION_MNU);
	stackVesselsList = gui->CreateListBox(_R(30, 320, int(width * 0.6), height - 10), MD_CONSTRUCTION_MNU, MD_CM_STACKED_VESSELS_LIST);
	gui->CreateDynamicButton("update stack", _R(int(width * 0.6) + 10, 320, int(width * 0.6) + 130, 345), MD_CONSTRUCTION_MNU, MD_CM_UPDATE_STACK_BTN);
	gui->CreateDynamicButton("assemble all", _R(int(width * 0.6) + 10, 350, int(width * 0.6) + 130, 375), MD_CONSTRUCTION_MNU, MD_CM_ASSEMBLE_ALL_BTN);*/
	
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

void GUI_MainConstruct::mapLayoutIdentifiers()
{

}