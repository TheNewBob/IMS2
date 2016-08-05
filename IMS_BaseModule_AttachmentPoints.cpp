//contains all methods od IMS_Module dealing with AttachmentPoint managment

#include "Common.h"
#include "IMS_Orbiter_ModuleData.h"
#include "IMS_General_ModuleData.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "Rotations.h"
#include "IMS.h"
#include "Calc.h"




void IMS_Module::CreatePhysicalAttachmentPoints()
{
	//get the static attachment point definitions
	vector<IMSATTACHMENTPOINT> &ap = _orbiter_data->getAttachmentPoints();

	//check if there are already attachment points physically existing (in case of a split, for example)
	//if yes, merely reset the existing ones instead of creating them from scratch
	if (attachmentPoints.size() != 0)
	{
		assert(attachmentPoints.size() == ap.size());		//if only some of the points exist, something went terribly wrong
		//reset the existing attachment points to the modules current alignement rather than creating them from scratch
		resetAttachmentPoints();
	}
	//create attachment points from scratch
	else
	{
		for (vector<IMSATTACHMENTPOINT>::iterator i = ap.begin(); i != ap.end(); ++i)
		{
			IMSATTACHMENTPOINT *newAttPoint = new IMSATTACHMENTPOINT;

			newAttPoint->pos = (*i).pos;
			newAttPoint->dir = (*i).dir;
			newAttPoint->rot = (*i).rot;

			newAttPoint->id = (*i).id;
			newAttPoint->dockedVessel = NULL;
			newAttPoint->exists = true;
			newAttPoint->dockPort = NULL;
			newAttPoint->rotModifier = 0;
			newAttPoint->connectsTo = NULL;
			newAttPoint->isMemberOf = this;

			attachmentPoints.push_back(newAttPoint);
		}
	}
}

void IMS_Module::resetAttachmentPoints()
{
	vector<IMSATTACHMENTPOINT> &ap = _orbiter_data->getAttachmentPoints();
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		IMSATTACHMENTPOINT *p = attachmentPoints[i];
		IMSATTACHMENTPOINT &ref = ap[i];
		p->pos = ref.pos;
		p->dir = ref.dir;
		p->rot = ref.rot;

		p->id = ref.id;
		p->exists = true;
		p->dockPort = NULL;
		p->rotModifier = 0;
		p->connectsTo = NULL;
		p->isMemberOf = this;
	}
}

//returns attachment points in points
void IMS_Module::GetAttachmentPoints(vector<IMSATTACHMENTPOINT*> &OUT_points)
{
	OUT_points = attachmentPoints;
}

//appends own attachment points to points without modifying it
void IMS_Module::AddAttachmentPointsToList(vector<IMSATTACHMENTPOINT*> &IN_OUT_points)
{
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		IN_OUT_points.push_back(attachmentPoints[i]);
	}
}


void IMS_Module::TransformAttachmentPoints()
{

	vector<IMSATTACHMENTPOINT> &ap = _orbiter_data->getAttachmentPoints();

	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		if (Rotations::RequiresRotation(dir, rot))
		{	
			//position
			VECTOR3 newPos = ap[i].pos;
//			newPos = mul(dirMat, newPos);
			VECTOR3 finalPos = mul(rotationmatrix, newPos);
			//direction
			VECTOR3 newDir = ap[i].dir;
//			newDir = mul(dirMat, newDir);
			VECTOR3 finalDir = mul(rotationmatrix, newDir);
			//rotation
			VECTOR3 newRot = ap[i].rot;
//			newRot = mul(dirMat, newRot);
			VECTOR3 finalRot = mul(rotationmatrix, newRot);

			Calc::RoundVector(finalDir, 1000);
			Calc::RoundVector(finalRot, 1000);
			normalise(finalDir);
			normalise(finalRot);
			
			attachmentPoints[i]->pos = finalPos;
			attachmentPoints[i]->dir = finalDir;
			attachmentPoints[i]->rot = finalRot;			

		}
		//updating position to vessel relative rather than module relative
		attachmentPoints[i]->pos += pos;
		Calc::RoundVector(attachmentPoints[i]->pos, 1000);
	}
}

void IMS_Module::RotateAttachmentPoint(IMSATTACHMENTPOINT *att, double angle)
{
	att->rotModifier += angle;
	Rotations::RotateVector(att->rot, att->dir, angle);
	if (att->dockPort != NULL)
	{
		vessel->SetDockParams(att->dockPort, att->pos, att->dir, att->rot);
		vessel->SetForRedock();
		vessel->Undock(vessel->GetDockIndex(att));
	}
}



/* updates state of attachment points and associated dockports as well as vessels docked to those ports
 * redock == true signifies that registered, but not docked vessels should be redocked (after splits or assimilations)
 * redock == false signifies that registered, but not docked vessels should be unregistered (after normal undockings)
 * possible states:
 * dockport exists not...
 *		...but should exist. Create dockport
 * dockport exists... 
 *		...but shouldn't. delete and remove docked vessel reference if neccesary
 *		...and needs to redock, or has been undocked. redock the vessel or unregister it
 *		...and has unregistered vessel docked. register vessel as docked
 * dockport requires no modification		
 */ 
void IMS_Module::UpdateAttachmentPoints(bool redock)
{
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		IMSATTACHMENTPOINT *p = attachmentPoints[i];

		//check if the docked vessel is a reference to the vessel containing the module. This happens to assimilated modules right after integration
		if (p->dockedVessel != NULL && p->dockedVessel->vessel == vessel)
		{
			delete p->dockedVessel;
			p->dockedVessel = NULL;
		}
		//the dockport does not exist, but should
		if (p->exists && p->dockPort == NULL)
		{
			p->dockPort = vessel->CreateDock(vessel->GetPositionRelativeToCoG(p->pos), p->dir, p->rot);
		}

		//dockport exists
		if (p->dockPort != NULL)
		{
			//see if this dockport is occupied
			OBJHANDLE dockedobj = vessel->GetDockStatus(p->dockPort);
			//dockport shouldn't actually exist
			if (!p->exists)
			{
				//delete dockport
				vessel->DelDock(p->dockPort);
				p->dockPort = NULL;
				//remove docked vessel reference if there is one
				if (p->dockedVessel != NULL)
				{
					vessel->UnregisterDockedVessel(p->dockedVessel->vessel, p);
					p->dockedVessel = NULL;
				}
			}
			//there should be a vessel docked here, but there isn't.
			else if (p->dockedVessel != NULL && dockedobj == NULL)
			{
				//if the vessel needs to redock, redock it
				if (redock)
				{
					dockVessel(p->dockedVessel, p);
				}
				//if it doesn't need to be redocked, unregister it
				else
				{
					vessel->UnregisterDockedVessel(p->dockedVessel->vessel, p);
				}
			}
			//there's a vessel docked, but it's not registered yet
			else if (p->dockedVessel == NULL && dockedobj != NULL)
			{
				p->dockedVessel = vessel->RegisterDockedVessel(oapiGetVesselInterface(dockedobj), p);
			}
			//a vessel should be docked, and a vessel is docked. Whether it's the right vessel is a different question.
			else if (p->dockedVessel != NULL && dockedobj != NULL)
			{
				VESSEL *v = oapiGetVesselInterface(dockedobj);
				//the vessel docked is not the right one. replace it!
				//this is a special case that only happens under certain circumstances after splitting, by the way.
				if (v != p->dockedVessel->vessel)
				{
					vessel->UnregisterDockedVessel(p->dockedVessel->vessel, p);
					p->dockedVessel = vessel->RegisterDockedVessel(v, p);
				}
			}
		}
		//the attachment point doesn't exist anymore, but still thinks it has a vessel docked
		else if (p->dockPort == NULL && p->dockedVessel != NULL)
		{
			vessel->UnregisterDockedVessel(p->dockedVessel->vessel, p);
			p->dockedVessel = NULL;
		}
	}
}

//docks vessel to the attachment point
void IMS_Module::dockVessel(DOCKEDVESSEL *v, IMSATTACHMENTPOINT *p)
{
	UINT thisPortIndex = vessel->GetDockIndex(p);
	UINT thatPortIndex = vessel->GetDockIndex(p->dockedVessel->dockPortOther, p->dockedVessel->vessel);

	vessel->Dock(p->dockedVessel->vessel->GetHandle(), thisPortIndex, thatPortIndex, 1);
	//re-add the vessel to the docking list
	p->dockedVessel = vessel->RegisterDockedVessel(p->dockedVessel->vessel, p);
}


void IMS_Module::SetDockPortsToNull()
{
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		attachmentPoints[i]->dockPort = NULL;
	}
}


IMSATTACHMENTPOINT *IMS_Module::GetConnectingAttachmentPoint(VESSEL *_v)
{
	IMSATTACHMENTPOINT *point = NULL;
	for (UINT i = 0; i < attachmentPoints.size() && point == NULL; ++i)
	{
		if (attachmentPoints[i]->dockedVessel != NULL)
		{
			if (attachmentPoints[i]->dockedVessel->vessel == _v)
			{
				point = attachmentPoints[i];
			}
		}
	}
	return point;
}


IMSATTACHMENTPOINT *IMS_Module::GetConnectingAttachmentPoint(IMS_Module* module)
{
	for (UINT i = 0; i < attachmentPoints.size(); ++i)
	{
		if (attachmentPoints[i]->connectsTo != NULL &&				//check if this attachment point is connected to anything at all
			attachmentPoints[i]->connectsTo->isMemberOf == module)	//check if the attached module is the one we are loocking for
		{
			return attachmentPoints[i];
		}
	}
	return NULL;
}


void IMS_Module::RemoveAllAttachmentPoints()
{
	for (vector<IMSATTACHMENTPOINT*>::iterator i = attachmentPoints.begin(); i != attachmentPoints.end(); ++i)
	{
		IMSATTACHMENTPOINT* p = (*i);
		//disconnect the attachment point that connects to this one
		if (p->connectsTo != NULL)
		{
			p->connectsTo->connectsTo = NULL;
			p->connectsTo->exists = true;
			p->connectsTo = NULL;
		}
		//if another vessel is docked to the port, remove it from the vessels docklist (but keep the attachment point connected and the DockedVessel object intact)
		if (p->dockedVessel != NULL)
		{
			vessel->UnregisterDockedVessel(p->dockedVessel->vessel, p, false);
		}

		//delete the dockingport from the vessel if one exists
		if (p->dockPort != NULL)
		{
			vessel->DelDock(p->dockPort);
		}
	}
}


int IMS_Module::GetAttachmentPointIndex(IMSATTACHMENTPOINT *point)
{
	vector<IMSATTACHMENTPOINT*>::iterator i = find(attachmentPoints.begin(), attachmentPoints.end(), point);
	if (i == attachmentPoints.end()) return -1;
	return i - attachmentPoints.begin();
}