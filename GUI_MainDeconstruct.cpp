#include "GUI_Common.h"
#include "Common.h"
#include "GUI_Elements.h"
#include "GUI_MainDeconstruct.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"
#include "IMS_InputCallback.h"
#include "IMS_Module.h"
#include "SimpleShape.h"
#include "IMS_General_ModuleData.h"
#include "IMS_Algs.h"
#include "LayoutManager.h"

using namespace IMS_Algs;

const static string FILENAME = "mainmenu/deconstruct.xml";
const static string MODULES_IN_VESSEL_LABEL = "modules_in_vessel_label";
const static string MODULES_IN_VESSEL_LIST = "modules_in_vessel_list";
const static string SORT_X_BTN = "sort_x_btn";
const static string SORT_Y_BTN = "sort_y_btn";
const static string SORT_Z_BTN = "sort_z_btn";
const static string SYMMETRY_CHKBX = "symmetry_chkbx";
const static string SELECTED_MODULES_LABEL = "selected_modules_label";
const static string SELECTED_MODULES_LIST = "selected_modules_list";
const static string DISASSEMBLE_BTN = "disassemble_btn";


GUI_MainDeconstruct::GUI_MainDeconstruct(RECT mRect, GUI_ElementStyle *_style, IMS2 *_vessel)
	: GUI_Page(mRect, MD_DECONSTRUCTION_MNU, _style), vessel(_vessel)
{
	GUImanager *gui = vessel->GetGUI();
	//register element in manager BEFORE adding children
	gui->RegisterGuiElement(this, GUI_MAIN_DISPLAY);

	LAYOUTCOLLECTION *layouts = LayoutManager::GetLayout(FILENAME);

	//docked vessels operations
	gui->CreateLabel(getLayoutDataForElement(MODULES_IN_VESSEL_LABEL, layouts), "modules in vessel", MD_DECONSTRUCTION_MNU);
	modules_list = gui->CreateListBox(getLayoutDataForElement(MODULES_IN_VESSEL_LIST, layouts), MD_DECONSTRUCTION_MNU, MD_DM_MODULES_LIST);


	gui->CreateDynamicButton(getLayoutDataForElement(SORT_X_BTN, layouts), "sort by X", MD_DECONSTRUCTION_MNU, MD_DM_SORTX_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(SORT_Y_BTN, layouts), "sort by Y", MD_DECONSTRUCTION_MNU, MD_DM_SORTY_BTN);
	gui->CreateDynamicButton(getLayoutDataForElement(SORT_Z_BTN, layouts), "sort by Z", MD_DECONSTRUCTION_MNU, MD_DM_SORTZ_BTN);
	symmetry_box = gui->CreateCheckBox(getLayoutDataForElement(SYMMETRY_CHKBX, layouts), "symmetrical", MD_DECONSTRUCTION_MNU, MD_DM_SYMMETRY_CHKBX);

	//stacked vessels operations
	gui->CreateLabel(getLayoutDataForElement(SELECTED_MODULES_LABEL, layouts), "selected modules", MD_DECONSTRUCTION_MNU);
	selected_modules_list = gui->CreateListBox(getLayoutDataForElement(SELECTED_MODULES_LIST, layouts), MD_DECONSTRUCTION_MNU, MD_DM_SELECTED_MODULES_LIST);
	gui->CreateDynamicButton(getLayoutDataForElement(DISASSEMBLE_BTN, layouts), "disassemble", MD_DECONSTRUCTION_MNU, MD_DM_DECONSTRUCT_BTN);

//	gui->CreateDynamicButton("set staging", _R(int(width * 0.6) + 10, 350, int(width * 0.6) + 130, 375), MD_DECONSTRUCTION_MNU, MD_DM_STAGING_BTN);

	UpdateModulesList();

}


GUI_MainDeconstruct::~GUI_MainDeconstruct()
{
}

void GUI_MainDeconstruct::UpdateModulesList(SORT_MODULE_BY sort_by, bool symmetric)
{
	//refresh the modules list
	modules.clear();
	vessel->GetModules(modules);

	//sort modules by axis
	sortedmodules.clear();
	sortedmodules.reserve(modules.size());
	//copy the modules from modules to sortedmodules, but without any modules that lie on 0 on the sort axis
	int nummodules = 0;
	for (UINT i = 0; i < modules.size(); ++i)
	{
		VECTOR3 pos;
		modules[i]->GetPos(pos);
		switch (sort_by)
		{
		case X_AXIS:
			if (pos.x != 0)
			{
				sortedmodules.push_back(modules[i]);
			}
			break;
		case Y_AXIS:
			if (pos.y != 0)
			{
				sortedmodules.push_back(modules[i]);
			}
			break;
		case Z_AXIS:
			if (pos.z != 0)
			{
				sortedmodules.push_back(modules[i]);
			}
			break;
		}
	}

	//now sort the order of the modules
	switch (sort_by)
	{
	case X_AXIS:
		if (symmetric) sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::X_SYMMETRIC_ORDER);
		else sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::X_ORDER);
		selected_modules.clear();
		selected_modules_list->clear();
		break;
	case Y_AXIS:
		if (symmetric) sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::Y_SYMMETRIC_ORDER);
		else sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::Y_ORDER);
		selected_modules.clear();
		selected_modules_list->clear();
		break;
	case Z_AXIS:
		if (symmetric) sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::Z_SYMMETRIC_ORDER);
		else sort(sortedmodules.begin(), sortedmodules.end(), IMS_Module::Z_ORDER);
		selected_modules.clear();
		selected_modules_list->clear();
		break;
	}


	//write the module names and positions into the listbox
	modules_list->clear();
	for (UINT i = 0; i < sortedmodules.size(); ++i)
	{
		modules_list->AddElement(sortedmodules[i]->getModulePosDescription());
	}
}



/* the select modules function controls the algorithm that selects which modules are going to be split off
 * This process is tough to understand: There are two core concepts: The CORE module, which is the module at the vessel
 * origin (and the first entry in the vessels modules list), and the TRIGGER module, which is the module the player
 * chooses to split off. The algorithm works by splitting the vessel with a plane perpendicular to the sorted axis,
 * at the selected module going outwards.
 * The algorithm first has to look for modules that are lying below that threshold, while being connected to the CORE
 * of the vessel. These modules will certainly STAY with the original vessel. If we omit this step, we can get modules
 * that stay with the original vessel because they lie below the threshold, even though they are only connected to the 
 * core via modules that lie above it and get split off. Such modules would be left "hanging in the air" with no connection
 * to the rest of the vessel.
 * Once we have the modules that will certainly stay with the ship, we can get a list of modules that might be split off by 
 * excluding all modules that are staying.
 * But now we have a list of all modules beyond the threshold and modules connected to them, however, there is no guarantee
 * that they all connect to the TRIGGER module. If we split the vessel like that, we get modules hanging in the air on the 
 * NEW vessel. So we again need to check for connections, this time starting from the TRIGGER. The additional condition this
 * time is that they are on the list of potentially split modules. The result of this last operation is a list of modules
 * that contains all modules that are a) connected to the TRIGGER and b) either beyond the split threshold OR not connected
 * to the core. I.E. we get a bunch of modules that lay from the selected modules towards the outside, form one connected body
 * and also don't leave anything hanging on the original vessel.
 *
 * selected_index: the index of the selected module from the ListBox
 */
void GUI_MainDeconstruct::selectModules(int selected_index)
{
	//get the core module and its position
	VECTOR3 corepos;
	IMS_Module *coremodule = vessel->GetCoreModule();
	coremodule->GetPos(corepos);
	selected_modules.clear();
	
	//get axis distance of trigger module to core module
	VECTOR3 triggerpos;
	sortedmodules[selected_index]->GetPos(triggerpos);
	double triggerdistance = getAxisDistanceBetween(triggerpos, corepos);
	//the module cannot lie on the axis with the core module, otherwise we won't know where to split
	if (triggerdistance == 0) return;
	
	//this list will contain all modules taht will REMAIN with the current vessel
	vector<IMS_Module*> checkedmodules;
	//start recursive search for modules connecting to the core module while being on the close side of the split threshold
	selectModulesConnectingTo(coremodule, checkedmodules, triggerdistance, corepos, CORE);

	//now that we have a list with all modules remaining on this vessel, convert it to a list of modules that are possibly going to be split off
	selected_modules = Vector_Difference(modules, checkedmodules);

	//and now, see which of these actually connect to the trigger
	checkedmodules.clear();
	selectModulesConnectingTo(sortedmodules[selected_index], checkedmodules, triggerdistance, corepos, TRIGGER);
	checkedmodules.shrink_to_fit();
	selected_modules = checkedmodules;


	//update the selected modules list (the gui list box, not the vector)
	selected_modules_list->clear();
	for (UINT i = 0; i < selected_modules.size(); ++i)
	{
		selected_modules_list->AddElement(selected_modules[i]->getModulePosDescription());
	}
}



/* Recursively Checks a module and its neighbours whether they qualifies as connecting to the CORE or the TRIGGER
 * module: the current module
 * checkedmodules: the list of modules that have checked out as qualifying
 * triggerdistance: The distance of the trigger to the sort axis (only distance from AXIS, i.e. not 3D distance
 * corepos: Position of the CORE module
 * connecting_to: whether the function works off of the CORE or the TRIGGER
 */
void GUI_MainDeconstruct::selectModulesConnectingTo(IMS_Module *module, vector<IMS_Module*> &checkedmodules, 
														double triggerdistance, VECTOR3 &corepos, CONNECTING_TO connecting_to)
{
	vector<IMSATTACHMENTPOINT*> points;
	module->GetAttachmentPoints(points);
	bool isselected = false;


	//if we are looking for modules connecting to the core that are NOT being split off...
	if (connecting_to == CORE)
	{
		VECTOR3 connectedpos;
		module->GetPos(connectedpos);
		//check if the module is on the close side of the trigger, and select if yes
		isselected = !isBeyondTriggerdistance(connectedpos, corepos, triggerdistance);
	}
	//if we are looking for modules connecting to the trigger, and this one is already on the narrow list, select it
	else if (connecting_to == TRIGGER && 
			 find(selected_modules.begin(), selected_modules.end(), module) != selected_modules.end())
	{
		isselected = true;
	}

	//if the module is selected, put it in the list, and look for modules connecting to it
	if (isselected)
	{
		checkedmodules.push_back(module);

		//walk through attachment points to check connected modules
		for (UINT i = 0; i < points.size(); ++i)
		{
			//check if the attachment point connects to another module
			IMSATTACHMENTPOINT *connectedpoint = points[i]->connectsTo;
			if (connectedpoint != NULL)
			{
				//check if the connecting module is already selected. Since we can have circular connections, just a compare to the caller won't do
				IMS_Module *connectedmodule = connectedpoint->isMemberOf;
				if (find(checkedmodules.begin(), checkedmodules.end(), connectedmodule) == checkedmodules.end())
				{
					selectModulesConnectingTo(connectedmodule, checkedmodules, triggerdistance, corepos, connecting_to);
				}
			}
		}
	}
}




/* returns the distance between a and b in the currently sorted axis
 * a: position of the TRIGGER
 * b: position of the CORE
 */
double GUI_MainDeconstruct::getAxisDistanceBetween(VECTOR3 a, VECTOR3 b)
{
	switch (sorting_order)
	{
	case X_AXIS:
		return a.x - b.x;
	case Y_AXIS:
		return a.y - b.y;
	default:
		return a.z - b.z;
	}
}


bool GUI_MainDeconstruct::isBeyondTriggerdistance(VECTOR3 a, VECTOR3 b, double triggerdistance)
{
	//calculate the relative position of a to b in the sorted axis
	double axisdistance = getAxisDistanceBetween(a, b);
	bool isbeyond = false;
	if (symmetry_box->Checked())
	{
		//the split should be symmetrical, it's enough if we compare absolute values
		isbeyond = abs(axisdistance) >= abs(triggerdistance);
	}
	else
	{
		//the split does not happen symmetricaly, so the sign has to be taken into consideration
		if (triggerdistance < 0)
		{
			//triggerdistance is negative, anything equal OR SMALLER is eligible for a split
			isbeyond = axisdistance <= triggerdistance;
		}
		else
		{
			//triggerdistance is positive, anything equal OR LARGER is eligible for a split
			isbeyond = axisdistance >= triggerdistance;
		}
	}
	return isbeyond;
}


int GUI_MainDeconstruct::ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	//get the clicked element
	int eventId = GUI_BaseElement::ProcessChildren(_event, _x, _y);

	switch (eventId)
	{
	case MD_DM_SORTX_BTN:		//sort by X
		UpdateModulesList(X_AXIS, symmetry_box->Checked());
		sorting_order = X_AXIS;
		break;
	case MD_DM_SORTY_BTN:		//sort by y
		UpdateModulesList(Y_AXIS, symmetry_box->Checked());
		sorting_order = Y_AXIS;
		break;
	case MD_DM_SORTZ_BTN:		//sort by z
		UpdateModulesList(Z_AXIS, symmetry_box->Checked());
		sorting_order = Z_AXIS;
		break;
	case MD_DM_MODULES_LIST:	//module selected
	{
		int selected = modules_list->GetSelected();
		if (selected != -1) selectModules(selected);
		break;
	}
	case MD_DM_DECONSTRUCT_BTN:	//deconstruct selected modules
		vessel->Split(selected_modules);
		UpdateModulesList(sorting_order, symmetry_box->Checked());
		break;
	}

	return eventId;
}

void GUI_MainDeconstruct::SetVisible(bool visible)
{
	GUI_BaseElement::SetVisible(visible);
	if (visible)
	{
		UpdateModulesList();
	}
}