#include "common.h"
#include "events.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "Calc.h"
#include "BoundingBox.h"
#include "SimpleShape.h"
#include "IMS_TouchdownPointManager.h"


const double IMS_TouchdownPointManager::TD_STIFFNESS = 1e7;
const double IMS_TouchdownPointManager::TD_DAMPING = 1e5;
const double IMS_TouchdownPointManager::TD_LATFRICTION = 3.0;
const double IMS_TouchdownPointManager::TD_LONGFRICTION = 3.0;


IMS_TouchdownPointManager::IMS_TouchdownPointManager(IMS2 *vessel)
	: IMS_Manager_Base(vessel)
{
}


IMS_TouchdownPointManager::~IMS_TouchdownPointManager()
{
	if (hullbox != NULL)
	{
		delete hullbox;
	}
}


void IMS_TouchdownPointManager::AddHullShape(SimpleShape *shape, VECTOR3 pos, MATRIX3 rot)
{
	//add the new hullshape to the list
	shapes.push_back(SHAPEDATA(shape, pos, rot));

	addHullShape(shape, pos, rot);
}


void IMS_TouchdownPointManager::addHullShape(SimpleShape *shape, VECTOR3 pos, MATRIX3 rot)
{
	//create a new shape and move transform it into vessel coordinates
	SimpleShape transformedshape(*shape);
	transformedshape.Rotate(rot);
	transformedshape.Translate(pos);

	//convert the shape vertices to touchdown points
	vector<TOUCHDOWNVTX> tdvertices;
	vector<VECTOR3> shapevertices;
	transformedshape.GetVertices(shapevertices);
	tdvertices.resize(shapevertices.size());

	//initialise the touchdown points
	for (UINT i = 0; i < shapevertices.size(); ++i)
	{
		tdvertices[i].pos = shapevertices[i];
		tdvertices[i].stiffness = TD_STIFFNESS;
		tdvertices[i].mu = TD_LATFRICTION;
		tdvertices[i].mu_lng = TD_LONGFRICTION;
		tdvertices[i].damping = TD_DAMPING;
	}

	//add touchdown points to the list if they are not duplicated
	//this is somewhat inefficient, might cause problems for Dantassii-sized vessels. 
	//If it turns out to be unmanagable, I'll have to write up a 3-d tree.
	for (UINT i = 0; i < tdvertices.size(); ++i)
	{
		bool addpoint = true;
		for (UINT j = 0; j < hullpoints.size(); ++j)
		{
			//if the point overlaps with an already existing point, don't add it!
			if (Calc::IsNear(tdvertices[i].pos, hullpoints[j].pos, 0.1))
			{
				addpoint = false;
				break;
			}
		}
		
		//if the point has no overlap, add it to the touchdown point list
		if (addpoint)
		{
			hullpoints.push_back(tdvertices[i]);
		}
	}

	//create or update the hulls bounding box
	if (hullbox == NULL)
	{
		hullbox = new BoundingBox(transformedshape.GetBoundingBox());
	}
	else
	{
		hullbox->MergeBoundingBox(transformedshape.GetBoundingBox());
	}

	//tell the manager to update the tdpoints in poststep
	addEventToWaitingQueue(new TdPointsChangedEvent);
}


void IMS_TouchdownPointManager::RemoveHullShape(SimpleShape *shape)
{
	//remove the shape from the manager, but do not yet recreate the touchdown points.
	//otherwise there's a good chance we'll recreate them several times per frame.
	auto it = find(shapes.begin(), shapes.end(), shape);
	assert(it != shapes.end() && "Attempting to remove a hullshape that was never added!");

	shapes.erase(it);
	//add an event to the waiting queue to trigger recreation of touchdown points in poststep
	addEventToWaitingQueue(new HullShapeRemovedEvent());
}


void IMS_TouchdownPointManager::setTdPoints()
{
	//if this vessel has no hullbox defined, leave the default touchdown points created by orbiter on initialisation.
	if (hullbox == NULL)
	{
		return;
	}

	//set the default 3 touchdown points
	vector<TOUCHDOWNVTX> defaultpoints;
	getDefaultTdPoints(defaultpoints);

	UINT totalpoints = 3 + hullpoints.size();

	//copy all points to an array and move them to CoG-relative position
	TOUCHDOWNVTX* tdarray = new TOUCHDOWNVTX[totalpoints];
	//the default points must go first!
	tdarray[0] = defaultpoints[0];
	tdarray[1] = defaultpoints[1];
	tdarray[2] = defaultpoints[2];

	//now come all the static hullpoints on the vessel
	VECTOR3 cogoffset = vessel->GetCoG();
	for (UINT i = 0; i < hullpoints.size(); ++i)
	{
		tdarray[i + 3] = hullpoints[i];
		tdarray[i + 3].pos -= cogoffset;
	}

	vessel->SetTouchdownPoints(tdarray, (DWORD)totalpoints);
	delete tdarray;
}


bool IMS_TouchdownPointManager::ProcessEvent(Event_Base *e)
{
	if (*e == HULLSHAPEREMOVEDEVENT)
	{
		//destroy all touchdown points
		hullpoints.clear();
		//destroy the bounding box
		delete hullbox;
		hullbox = NULL;

		//recreate the overall hullshape with the shapes we have left over
		for (UINT i = 0; i < shapes.size(); ++i)
		{
			AddHullShape(shapes[i].shape, shapes[i].pos, shapes[i].rot);
		}
		return true;
	}
	else if (*e == TDPOINTSCHANGEDEVENT)
	{
		//reset the touchdown points in orbiter
		setTdPoints();
		return true;
	}
	return false;
}

void IMS_TouchdownPointManager::getDefaultTdPoints(vector<TOUCHDOWNVTX> &OUT_points)
{
	//no landing gear was added, we default to three td-points at the bottom of the bounding box,
	//z+ forward and y+ up.

	double left = hullbox->GetLeft();
	double right = hullbox->GetRight();
	double bottom = (hullbox->GetBottom());
	double rear = hullbox->GetRear();
	double front = hullbox->GetFront();

	OUT_points.resize(3);
	//set default attributes of all points
	TOUCHDOWNVTX vtx;
	vtx.damping = TD_DAMPING;
	vtx.mu = TD_LATFRICTION;
	vtx.mu_lng = TD_LONGFRICTION;
	vtx.stiffness = TD_STIFFNESS;


	//create the three default points. Note that the order is very important!
	//also, we immediately switch them to CoG-relative positions. They are only needed
	//by orbiter itself, after all.

	VECTOR3 cogoffset = vessel->GetCoG();
	vtx.pos = _V((left + right) / 2, bottom, front) - -cogoffset;
	OUT_points[0] = vtx;
	vtx.pos = _V(left, bottom, rear) - cogoffset;
	OUT_points[1] = vtx;
	vtx.pos = _V(right, bottom, rear) - cogoffset;
	OUT_points[2] = vtx;
}