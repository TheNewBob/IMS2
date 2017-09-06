#include "Common.h"
#include "GUI_Common.h"
#include "GUIentity.h"
#include "GUImanager.h"
#include "Events.h"
#include "IMS.h"
#include "IMS_Module.h"
#include "GUI_Surface.h"
#include "GUI_MainDisplay.h"
#include "Calc.h"


void IMS2::CreateDockedVesselsList()
{

}

/* removes or adds a docked vessel to this vessels docked vessels list
 * vessel: the actual VESSEL pointer of the other vessel
 * point: IMSATTACHMENTPOINT vessel was docked to
 * remove: pass true if the vessel should be removed permanently from the list. Default: false
 * dealloc: pass false if the DockedVessel object should not be deleted after removing it from the list. 
 * This is only necessary to preserve docking information when a vessel is split. default: true*/

/*DOCKEDVESSEL *IMS2::UpdateDockedVesselsList(VESSEL *vessel, IMSATTACHMENTPOINT *point, bool remove, bool dealloc)
{

	//add the docking connection to the list
	else
	{
		//When a vessel splits, the attachment point will still hold a by now outdated VesselObject
		//if we don't delete it now, we'll lose it.
		if (point->dockedVessel != NULL)
		{
			delete point->dockedVessel;
		}
		return newVes;
	}
	return NULL;
}*/


DOCKEDVESSEL *IMS2::RegisterDockedVessel(VESSEL *vessel, IMSATTACHMENTPOINT *point)
{
	//first, check if the vessel isn't already registered
	DOCKEDVESSEL *newves = GetDockedVessel(vessel);
	
	if (newves == NULL)
	{
		newves = new DOCKEDVESSEL;
		newves->attachedTo = point;
		newves->vessel = vessel;
		newves->type = GetVesselType(vessel);
		//find the connecting dockport on the other vessel
		for (UINT i = 0; i < vessel->DockCount(); ++i)
		{
			DOCKHANDLE dh = vessel->GetDockHandle(i);
			if (vessel->GetDockStatus(dh) == this->GetHandle())
			{
				newves->dockPortOther = dh;
			}
		}
		dockedVesselsList.push_back(newves);
	}
	return newves;
}

DOCKEDVESSEL *IMS2::GetDockedVessel(VESSEL *vessel)
{
	vector<DOCKEDVESSEL*>::iterator i = find_if(dockedVesselsList.begin(), dockedVesselsList.end(), DockedVesselToVesselComparator(vessel));
	if (i == dockedVesselsList.end())
	{
		//the passed vessel is not docked to this
		return NULL;
	}
	return (*i);
}


/* unregisters a DOCKEDVESSEL object from this vessel if one is found
 * if none is found, the attachment point will still be freed if dealloc is true
 * vessel: a pointer to the VESSEL object contained by the DOCKEDVESSEL to be unregistered (NOT the DOCKEDVESSEL object itself! that one is ambiguous!)
 * point: The attachment the vessel is docked to
 * dealloc: default true; pass true if the DOCKEDVESSEL object should only be removed from the vessel list, but NOT deallocated. Usually necessary for splits
 */
void IMS2::UnregisterDockedVessel(VESSEL *vessel, IMSATTACHMENTPOINT *point, bool dealloc)
{
	//check if the vessel is on the docking list, remove if yes
	for (UINT i = 0; i < dockedVesselsList.size(); ++i)
	{
		if (dockedVesselsList[i]->vessel == vessel)
		{
			//do not delete the object if the caller thinks it's still needed (usually the case when splitting)
			if (dealloc)
			{
				delete dockedVesselsList[i];
				point->dockedVessel = NULL;
			}
			dockedVesselsList.erase(dockedVesselsList.begin() + i);
			dockedVesselsList.shrink_to_fit();
			return;
		}
	}
	
	//if the vessel was not found in the docked vessels list, but it should be deallocated, clean up the attachment point
	if (dealloc && point->dockedVessel != NULL)
	{
		delete point->dockedVessel;
		point->dockedVessel = NULL;
	}
}


void IMS2::UpdateDockPorts(bool hasAssimilated)
{
	//connect neighboring attachment points on this vessel
	ConnectAttachmentPoints();
	//update the state of the attachment points on each module
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->UpdateAttachmentPoints(hasAssimilated);
	}

	if (hasAssimilated) mainDispSurface->UpdateModulesList();
}

//establishes connection between attachment points of INTEGRATED modules (circular connections possible!)
void IMS2::ConnectAttachmentPoints()
{
	//gather up all attachment points of the vessel
	vector<IMSATTACHMENTPOINT*> points;
	//we assume that every module has at least 3 attachment points on average to save some time with allocation.The memory won't be needed for long anyways.
	points.reserve(modules.size() * 3);					
	
	//walk through all modules and collect their attachment points
	for (UINT i = 0; i < modules.size(); ++i)
	{
		modules[i]->AddAttachmentPointsToList(points);
	}
	if (points.size() < 1) return;

	sort(points.begin(), points.end(), IMS_Module::ATTACHMENT_POINTS_BY_POS);

	for (UINT i = 0; i < points.size() - 1; ++i)
	{
		//check if this point and its neighbor overlap
		if (Calc::IsNear(points[i]->pos, points[i + 1]->pos, 0.01))
		{
			//connect the points
			points[i]->connectsTo = points[i + 1];
			points[i + 1]->connectsTo = points[i];
			points[i]->exists = false;
			points[i + 1]->exists = false;
			//since  i + 1 already connects to i, it cannot connect to i + 2. increment i for efficiency.
			++i;
		}
	}
}

void IMS2::clbkDockEvent(int dock, OBJHANDLE mate)
{
	if (modules.size() > 0)
	{
		//vessels with no modules were assimilated in the last frame and are waiting
		//for deletion in this. They still get a dockevent from Orbiter, but there's
		//no need to process it.

		addEvent(new IMS_DockEvent);
	}
}




void IMS2::HandleDockEvent()
{
	UpdateDockPorts(isSetForRedock);
	isSetForRedock = false;
	mainDispSurface->UpdateDockedVesselsList(dockedVesselsList);
	mainDispSurface->UpdateModulesList();

}

void IMS2::GetStack(std::vector<VESSEL*> &stackList, OBJHANDLE callingVessel)
//gets all IMS2 vessels in the current docking stack
//recursive function!
{
	for (UINT i = 0; i < dockedVesselsList.size(); ++i)
	{
		if (dockedVesselsList[i]->type == IMS_VESSEL && 
			dockedVesselsList[i]->vessel->GetHandle() != callingVessel)
		{
			IMS2 *imsv = (IMS2*)(dockedVesselsList[i]->vessel);
			imsv->GetStack(stackList, this->GetHandle());
			stackList.push_back(dockedVesselsList[i]->vessel);
		}
	}
}