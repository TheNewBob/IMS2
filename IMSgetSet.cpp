#include "Common.h"
#include "GUI_Common.h"
#include "Managers.h"
#include "IMS_CoGmanager.h"
#include "IMS_RcsManager.h"
#include "SimpleShape.h"
#include "IMS_TouchdownPointManager.h"
#include "IMS.h"
#include "IMS_Module.h"

//this file contains functions for getting and setting orbiter related attributes of the vessel.



VISHANDLE IMS2::GetVisHandle()
{
	return hVis;
}

GUImanager *IMS2::GetGUI()
{
	return GUI;
}




IMSATTACHMENTPOINT *IMS2::GetConnectingAttachmentPoint(VESSEL *_vessel)
//returns the attachmentpoint that connects to the passed vessel.
//NULL if passed vessel is not docked to this
//NOTE: should only be used to retrieve established connections. Cannot be used for the establishment of connections!
{
	IMSATTACHMENTPOINT *point = NULL;
	for (UINT i = 0; i < modules.size() && point == NULL; ++i)
	{
		point = modules[i]->GetConnectingAttachmentPoint(_vessel);
	}
	return point;
}



UINT IMS2::GetDockIndex(IMSATTACHMENTPOINT *ap)
{
	for (UINT i = 0; i < DockCount(); ++i)
	{
		if (GetDockHandle(i) == ap->dockPort)
		{
			return i;
		}
	}
	throw (INVALID_ARGUMENT_EXCEPTION);
}


UINT IMS2::GetDockIndex(DOCKHANDLE dockH)
{
	for (UINT i = 0; i < DockCount(); ++i)
	{
		if (GetDockHandle(i) == dockH)
		{
			return i;
		}
	}
	throw (INVALID_ARGUMENT_EXCEPTION);
}


//gets the dockindex of a dock on another vessel
UINT IMS2::GetDockIndex(DOCKHANDLE dockH, VESSEL *vessel)
{
	for (UINT i = 0; i < vessel->DockCount(); ++i)
	{
		if (vessel->GetDockHandle(i) == dockH)
		{
			return i;
		}
	}
	//do not handle this exception! It's here to alert devs when something with the dockport handling goes wrong.
	//if everything works as it is supposed to, this case should never happen in the first place.
	//if it does happen, the cause usually is that the connections between points has not been recognised,
	//which in turn is usually caused by bugs while calculating the module's position
	throw (invalid_argument("Dockport does not exist on any docked vessels!"));
}


vector<DOCKEDVESSEL*> &IMS2::GetDockedVesselsList()
{
	return dockedVesselsList;
}

VESSELTYPE IMS2::GetVesselType(VESSEL *_vessel)
//returns the type of the vessel
{
	if (_vessel->Version() >= 2) 
	{
		IMS2 *imsv = dynamic_cast<IMS2*>((VESSEL2*)_vessel);
		if (imsv != NULL)
		{
			return IMS_VESSEL;
		}
	}
	return DEF_VESSEL;
}

DOCKHANDLE IMS2::GetDockByStatus(OBJHANDLE dockedv)
{
	for (UINT i = 0; i < DockCount(); ++i)
	{
		DOCKHANDLE curDockHandle = GetDockHandle(i);
		if (GetDockStatus(curDockHandle) == dockedv)
		{
			return curDockHandle;
		}
	}
	return NULL;
}

//returns the attachment point connecting to the passe object, null if there's no connection
//contrary to GetConnectingAttachmentPoint, this function can be used for the establishment of connections
IMSATTACHMENTPOINT *IMS2::GetConnectingAttachmentPointByStatus(OBJHANDLE dockedv)
{
	for (UINT i = 0; i < modules.size(); ++i)
	{
		vector<IMSATTACHMENTPOINT*> points;
		modules[i]->GetAttachmentPoints(points);
		for (UINT j = 0; j < points.size(); ++j)
		{
			if (points[j]->dockPort != NULL && GetDockStatus(points[j]->dockPort) == dockedv)
			{
				return points[j];
			}
		}
	}
	return NULL;
}


//resizes the passed vector and fills it with the current module pointers. 
void IMS2::GetModules(vector<IMS_Module*> &OUT_modules)
{
	OUT_modules = modules;
}


void IMS2::SetForRedock()
//tells this vessel that it needs to redock undocked vessels 
{
	isSetForRedock = true;
}

VECTOR3 IMS2::GetCoG()
{
	return GetCoGmanager()->GetCoG();
}

VECTOR3 IMS2::GetPositionRelativeToCoG(VECTOR3 pos)
{
	return pos - GetCoGmanager()->GetCoG();
}

UINT IMS2::CreateAnim(double initial_state)
{
	//get the list of existing animations on the vessel
	ANIMATION *animlist;
	UINT nanim = GetAnimPtr(&animlist);

	//check the list for deleted entries (entries without a component).
	for (UINT i = 0; i < nanim; ++i)
	{
		if (animlist[i].ncomp == 0)
		{
			//an animation without components has been found.
			//return its id so it will be overwritten by the new animation's components
			animlist[i].defstate = initial_state;
			return i;
		}
	}

	//if there are no empty animations, create one
	UINT retvalue = CreateAnimation(initial_state);
	nanim = GetAnimPtr(&animlist);
	return retvalue;
}


IMS_PropulsionManager *IMS2::GetPropulsionManager()
{
	return (IMS_PropulsionManager*)managers[PROP_MANAGER];
}

IMS_CoGmanager *IMS2::GetCoGmanager()
{
	return (IMS_CoGmanager*)managers[COG_MANAGER];
}

IMS_RcsManager *IMS2::GetRcsManager()
{
	return (IMS_RcsManager*)managers[RCS_MANAGER];
}

IMS_TouchdownPointManager *IMS2::GetTdPointManager()
{
	return (IMS_TouchdownPointManager*)managers[TDPOINT_MANAGER];
}