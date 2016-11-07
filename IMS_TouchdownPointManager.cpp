#include "common.h"
#include "events.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "Calc.h"
#include "BoundingBox.h"
#include "SimpleShape.h"
#include "IMS_TouchdownPointManager.h"
#include "Rotations.h"

const double IMS_TouchdownPointManager::TD_STIFFNESS = 1e7;
const double IMS_TouchdownPointManager::TD_DAMPING = 1e5;
const double IMS_TouchdownPointManager::TD_LATFRICTION = 5.0;
const double IMS_TouchdownPointManager::TD_LONGFRICTION = 5.0;


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


UINT IMS_TouchdownPointManager::AddLandingTdPoint(VECTOR3 &pos, VECTOR3 &dir, double stiffness, double damping)
{
	if (landingpoint_id == UINT_MAX)
	{
		//I HOPE this is will always remain a joke!
		Helpers::writeToLog(string("Congratiulations! Over the course of constructing your vessel, you have added landing gear 4,294,967,296 times. In one single session. YOU.HAVE.GOT.TO.BE.KIDDING!"), L_ERROR);
		throw runtime_error("No effing way...");
	}

	TOUCHDOWNVTX newvert;
	newvert.stiffness = stiffness;
	newvert.damping = damping;
	newvert.mu = TD_LATFRICTION;
	newvert.mu_lng = TD_LONGFRICTION;
	newvert.pos = pos;
	
	landingpoint_id++;
	landingpoints[landingpoint_id] = TDDATA(newvert, dir);
	addEventToWaitingQueue(new TdPointsChangedEvent);
	return landingpoint_id;
}


void IMS_TouchdownPointManager::RemoveLandingTdPoint(UINT id)
{
	landingpoints.erase(landingpoints.find(id));
	addEventToWaitingQueue(new TdPointsChangedEvent);
}


void IMS_TouchdownPointManager::setTdPoints()
{
	UINT totalpoints = defaulttdtriangle.size() + hullpoints.size() + landingpoints.size();

	//copy all points to an array and move them to CoG-relative position
	TOUCHDOWNVTX* tdarray = new TOUCHDOWNVTX[totalpoints];
	VECTOR3 cogoffset = vessel->GetCoG();

	//the default points must go first!
	if (defaulttdtriangle.size() > 0)
	{
		tdarray[0] = defaulttdtriangle[0];
		tdarray[1] = defaulttdtriangle[1];
		tdarray[2] = defaulttdtriangle[2];

		tdarray[0].pos -= cogoffset;
		tdarray[1].pos -= cogoffset;
		tdarray[2].pos -= cogoffset;
	}

	//add all the gear points
	UINT idx = defaulttdtriangle.size();
	for (auto i = landingpoints.begin(); i != landingpoints.end(); ++i)
	{
		tdarray[idx] = i->second.vtx;
		tdarray[idx].pos -= cogoffset;
		idx++;
	}

	for (UINT i = 0; i < hullpoints.size(); ++i)
	{
		tdarray[i + idx] = hullpoints[i];
		tdarray[i + idx].pos -= cogoffset;
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
			addHullShape(shapes[i].shape, shapes[i].pos, shapes[i].rot);
		}
		return true;
	}
	else if (*e == TDPOINTSCHANGEDEVENT)
	{
		//The td-points have been created in PostLoad, but the event will still come in after the first frame.
		//processing it again at that point is unnecessary overhead, and would terminate the landed state of the vessel until equilibrium is reestablished.
		if (!firstTdPointsChangedEvent)
		{
			//create the landing triangle
			createDefaultTdTriangle();
			//reset the touchdown points in orbiter
			setTdPoints();
			return true;
		}
		else
		{
			firstTdPointsChangedEvent = false;
		}
	}
	return false;
}

vector<VECTOR3> IMS_TouchdownPointManager::createDefaultTdTriangleFromHullshape()
{
	//no landing gear was added, we default to three td-points at the bottom of the bounding box,
	//z+ forward and y+ up.

	double left = hullbox->GetLeft();
	double right = hullbox->GetRight();
	double bottom = (hullbox->GetBottom());
	double rear = hullbox->GetRear();
	double front = hullbox->GetFront();


	//create the three default points. Note that the order is very important!
	//also, we immediately switch them to CoG-relative positions. They are only needed
	//by orbiter itself, after all.

	defaulttdtriangle.resize(3);
	VECTOR3 cogoffset = vessel->GetCoG();

	vector<VECTOR3> triangle(3);

	triangle[0] = _V((left + right) / 2, bottom, front) - cogoffset;
	triangle[1] = _V(left, bottom, rear) - cogoffset;
	triangle[2] = _V(right, bottom, rear) - cogoffset;

	return triangle;
}


void IMS_TouchdownPointManager::createDefaultTdTriangle()
{
	defaulttdtriangle.clear();
	vector<VECTOR3> defaulttriangle;
	bool onlytriangle = false;			//changes to true if the default triangle is all there is in terms of touchdown points

	if (landingpoints.size() < 3)
	{
		if (hullbox == NULL)
		{
			//there's nothing to base an educated guess on, just leave the defaults
			defaulttriangle.resize(3);
			for (UINT i = 0; i < 3; ++i)
			{
				TOUCHDOWNVTX vtx;
				vessel->GetTouchdownPoint(vtx, i);
				defaulttriangle[i] = vtx.pos;
				onlytriangle = true;
			}
		}
		else
		{
			//not enough landing points defined to do anything smart, just go with the hullshape:
			defaulttriangle = createDefaultTdTriangleFromHullshape();
		}
	}
	else
	{
		defaulttriangle = createDefaultTdTriangleFromLandingGear();
	}

	defaulttdtriangle.resize(3);

	//set default attributes of all points
	TOUCHDOWNVTX vtx;
	if (onlytriangle)
	{
		//if there is only the triangle, set default damping and stiffness values
		vtx.damping = TD_DAMPING;
		vtx.stiffness = TD_STIFFNESS;
	}
	else
	{
		//otherwise, set stiffness and damping to 0, because it's just a virtual orientation direction for orbiter - 
		//the actual landing touchdownpoints will still be there, and the triangle will actually get in the way.
		vtx.damping = 0;
		vtx.stiffness = 0;
	}
	vtx.mu = TD_LATFRICTION;
	vtx.mu_lng = TD_LONGFRICTION;

	//set positions
	vtx.pos = defaulttriangle[0];
	defaulttdtriangle[0] = vtx;
	vtx.pos = defaulttriangle[1];
	defaulttdtriangle[1] = vtx;
	vtx.pos = defaulttriangle[2];
	defaulttdtriangle[2] = vtx;

}


vector<VECTOR3> IMS_TouchdownPointManager::createDefaultTdTriangleFromLandingGear()
{
	//the first challenge we face when having 3 or more touchdownpoints is exactly the same:
	//we need to be able to determine the relation of the touchdown points in the context of left, right, front and center,
	//in the correct order so the plane normal points the right way down. Unfortunately, this is easier said than done, 
	//since the understanding of those terms only have relevance in the plane of the touchdownpoints themselves, which might point anywhere.

	//we solve this problem by getting all the gear points, ASSUME they're at least all on the same plane, and rotate them so that plane would be "normal" to the vessel, i.e. down towards negative y.

	//matrix to rotate from origin (0 0 1) to down (0 -1 0). What we'll really need is the inverse of that matrix, however.
	MATRIX3 fromorigin = Rotations::GetRotationMatrixFromDirection(_V(0, -1, 0));
	//fromorigin = Rotations::InverseMatrix(fromorigin);

	VECTOR3 bugme1 = mul(fromorigin, _V(0, 0, 1));

	//matrix to rotate from tdpoint orientation to origin (0 0 1)
	//note that we will assume that all tdpoints are facing the same direction (they roughly should, or 
	//the engineer needs a slap). Otherwise the trigonometry could get very expensive.
	MATRIX3 toorigin = Rotations::GetRotationMatrixFromDirection(landingpoints.begin()->second.dir);

	VECTOR3 bugme2 = mul(toorigin, _V(0, 0, -1));

	//multiply the two matrices to get the overall rotation.
	MATRIX3 todown = mul(fromorigin, toorigin);

	VECTOR3 bugme3 = mul(todown, _V(0, 0, -1));

	//rotate all the landing points and shove them in a vector
	//now, the reson we do this is purely to get the points into a reference frame where we have a clear, 
	//2-dimensional definition of front, left and right. The result of the loop should be the 3 tdpoints appropriate for our triangle.
	//we'll also have to put them relative to the cog.
	vector<VECTOR3> rotatedpoints;
	rotatedpoints.reserve(landingpoints.size());
	VECTOR3 cogoffset = vessel->GetCoG();

	for (auto i = landingpoints.begin(); i != landingpoints.end(); ++i)
	{
		rotatedpoints.push_back(mul(todown, i->second.vtx.pos - cogoffset));
	}

	vector<VECTOR3> triangle(3);

	if (rotatedpoints.size() == 3)
	{
		//just compose the triangle of the 3 td points. We'll figure out the order at the end.
		triangle[0] = rotatedpoints[0];
		triangle[1] = rotatedpoints[1];
		triangle[2] = rotatedpoints[2];
	}
	else
	{
		//If we have more than 3 points, we must construct our triangle from the existing points, in a more or less reasonable way.
		//This is one of the things that you gloss over in planning, and then realise that it's the hardest thing in the code when you try to implement it...

		//sort the points by their horizontal distance from 0 0 0. 
		//This way we'll end up with the best candidates to form the triangle at the front of the vector.
		sort(rotatedpoints.begin(), rotatedpoints.end(), SORT_BY_HORIZONTAL_DISTANCE);

		//next, split that vector into 3 vectors containing left, right and center points
		vector<VECTOR3> centerpoints;
		vector<VECTOR3> leftpoints;
		vector<VECTOR3> rightpoints;

		for (UINT i = 0; i < rotatedpoints.size(); ++i)
		{
			if (Calc::IsNear(rotatedpoints[i].x, 0, 0.01))
			{
				centerpoints.push_back(rotatedpoints[i]);
			}
			else if (rotatedpoints[i].x < 0)
			{
				leftpoints.push_back(rotatedpoints[i]);
			}
			else
			{
				rightpoints.push_back(rotatedpoints[i]);
			}
		}

		//we have to guard against the attempt of somebody only adding landing gear on one side of the vessel, which one could conceivably
		//do for the lulz, but it is also a more likely occurance during construction. We don't want to crash if that happens!
		if (leftpoints.size() == 0 || rightpoints.size() == 0)
		{
			//in this case, just create the triangle from the hullshape
			return createDefaultTdTriangleFromHullshape();
		}

		VECTOR3 leftpoint = leftpoints[0];
		VECTOR3 rightpoint = rightpoints[0];
		VECTOR3 centerpoint;

		//there's a real possibility that there are no points in the center.
		//if there are, it's pretty simple: the center point furthest opposed to 
		//the left and right points is the third point of the triangle.
		if (centerpoints.size() > 0)
		{
			sort(centerpoints.begin(), centerpoints.end(), SORT_DESCENDING_BY_Z);
			if (Calc::IsNear(leftpoint.z, 0, 0.01 && centerpoints.size() >= 2))
			{
				//there is a problem where the left and right touchdown point might be at the center of mass in the z direction.
				//in fact, this problem is to be expected with 4-leged tailsitters. This will essentially lead to the default triangle
				//being unstable. So what we do is check if there is a center rear touchdown point, and if there is, make the left and right
				//points the average of that and the respective side point.
				leftpoint = (leftpoint + centerpoints[centerpoints.size() - 1]) / 2;
				rightpoint = (rightpoint + centerpoints[centerpoints.size() - 1]) / 2;
				centerpoint = centerpoints[0];
			}
			else if (leftpoint.z > 0)
			{
				//the side points are in front of the center of gravity, complement them with the rearmost center point.
				centerpoint = centerpoints[centerpoints.size() - 1];
			}
			else
			{
				//the side points are behind the center of gravity, complement them with the frontmost center point.
				centerpoint = centerpoints[0];
			}
		}
		else
		{
			//if there are no centerpoints, what we want is the horizontal average between the front- or rearmost left and right points
			sort(leftpoints.begin(), leftpoints.end(), SORT_DESCENDING_BY_Z);
			sort(rightpoints.begin(), rightpoints.end(), SORT_DESCENDING_BY_Z);
			if (leftpoint.z > 0)
			{
				centerpoint = (leftpoints[leftpoints.size() - 1] + rightpoints[rightpoints.size() - 1]) / 2;
			}
			else
			{
				centerpoint = (leftpoints[0] + rightpoints[0]) / 2;
			}
		}

		triangle[0] = centerpoint;
		triangle[1] = leftpoint;
		triangle[2] = rightpoint;
	}


	//now that we have 3 points, we need them in the right order so Orbiter gets where up is supposed to be.
	//determine up normal for the current order of points
	VECTOR3 normal = crossp(triangle[2] - triangle[0], triangle[1] - triangle[0]);
	if (normal.y < 0)
	{
		//the normal points down, add the points in reverse order.
		//if the normal points up, the points already are in the right order
		VECTOR3 swap = triangle[1];
		triangle[1] = triangle[2];
		triangle[2] = swap;
	}

	//offset the triangle by a tiny bit. If they are at the exact same height as the rest of
	//the touchdown points from landing gear, the result in orbiter is unstable!
/*	triangle[0].y += 0.1;
	triangle[1].y += 0.1;
	triangle[2].y += 0.1;*/

	//all that's left to do is rotate the points back to their original alignement and move them back to vessel relative coordinates
	for (UINT i = 0; i < 3; ++i)
	{
		triangle[i] = tmul(todown, triangle[i]);
		triangle[i] += cogoffset;
		Calc::RoundVector(triangle[i], 1000);
	}
	return triangle;
}


bool IMS_TouchdownPointManager::SORT_BY_HORIZONTAL_DISTANCE(VECTOR3 &a, VECTOR3 &b)
{
	double dista = abs(a.x + a.z);
	double distb = abs(b.x + b.z);
	
	if (dista == distb)
	{
		//if the points are perfectly symmetrical, the point that has a negative z-axis wins.
		//this way points at the rear of the vessel get preference, insuring sideways symmetrical points end up next to each other after sorting.
		return a.z < b.z;
	}

	return  dista > distb;
}


bool IMS_TouchdownPointManager::SORT_DESCENDING_BY_Z(VECTOR3 &a, VECTOR3 &b)
{
	return a.z > b.z;
}


void IMS_TouchdownPointManager::PostLoad()
{
	//the touchdown points must be set before clbkPostCreation, otherwise orbiter can't assign landed state to the vessel.
	createDefaultTdTriangle();
	setTdPoints();
}