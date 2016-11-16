#include "common.h"
#include "events.h"
#include "IMS.h"
#include <thread>
#include "IMS_Manager_Base.h"
#include "IMS_RcsManager.h"
#include "FiringSolutionThruster.h"
#include "FSThrusterCollection.h"
#include "FiringSolutionCalculator.h"
#include "Calc.h"
#include "IMS_Matrix.h"


FIRING_SOLUTION & FIRING_SOLUTION::operator*=(double rhs)
{
	for (auto it = coefficents.begin(); it != coefficents.end(); ++it)
	{
		it->second *= rhs;
	}
	return *this;
}

const FIRING_SOLUTION FIRING_SOLUTION::operator*(double rhs)
{
	FIRING_SOLUTION result = *this;
	result *= rhs;
	return result;
}

FIRING_SOLUTION & FIRING_SOLUTION::operator+=(const FIRING_SOLUTION& rhs)
{
	for (auto it = coefficents.begin(); it != coefficents.end(); ++it)
	{
		auto rhsit = rhs.coefficents.find(it->first);
		if (rhsit != rhs.coefficents.end())
		{
			it->second += rhsit->second;
		}
	}
	return *this;
}



FiringSolutionCalculator::FiringSolutionCalculator(IMS2 *vessel, vector<THRUSTER_HANDLE> rcsthrusters)
	: vessel(vessel), thrusterhandles(rcsthrusters)
{
	//start the calculation in a new thread
	Helpers::writeToLog(string("Starting Rcs calculation"), L_DEBUG);
	calculationthread = new thread(&FiringSolutionCalculator::calculateFiringSolutions, this);
}


FiringSolutionCalculator::~FiringSolutionCalculator()
{
	//if the thread is still running, kill it ASAP
	if (calculationthread->joinable())
	{
		abort = true;
		calculationthread->join();
		delete calculationthread;
	}
	if (thrusters != NULL)
	{
		delete thrusters;
	}
}


//thrust is between 0 and 1
void FiringSolutionCalculator::Apply(VECTOR3 &torque, VECTOR3 &force, double thrust)
{
	if (solutionready)
	{
		//FIRING_SOLUTION sol = scaleFiringSolution(constructFiringSolution(force, torque));
		FIRING_SOLUTION sol = constructFiringSolution(force, torque);
		for (auto it = sol.coefficents.begin(); it != sol.coefficents.end(); ++it)
		{
			vessel->SetThrusterLevel(it->first, it->second * thrust);
		}
	}
}


void FiringSolutionCalculator::CalculateTorqueLevels(VECTOR3 &torque)
{
	if (solutionready)
	{
		//first, determine which groups will be involved in the maneuver, 
		//and the maximum thrust they can produce around their axes.
		vector<THGROUP_TYPE> involvedgroups = getGroupsFromForceVector(torque, F_TORQUE);
		VECTOR3 maxforce = _V(0, 0, 0);
		for (UINT i = 0; i < involvedgroups.size(); ++i)
		{
			VECTOR3 &f = maxforces[involvedgroups[i]][F_TORQUE];
			if (f.x > maxforce.x) maxforce.x = f.x;
			if (f.y > maxforce.y) maxforce.y = f.y;
			if (f.z > maxforce.z) maxforce.z = f.z;
		}

		//now that we have the max torque we can possibly produce, let's convert the torque into thrust levels
		VECTOR3 level;
		level.x = min(1, abs(torque.x) / maxforce.x);
		level.y = min(1, abs(torque.y) / maxforce.y);
		level.z = min(1, abs(torque.x) / maxforce.x);

		torque.x = (torque.x < 0) ? torque.x = level.x * -1 : torque.x = level.x;
		torque.y = (torque.y < 0) ? torque.y = level.y * -1 : torque.y = level.y;
		torque.z = (torque.z < 0) ? torque.z = level.z * -1 : torque.z = level.z;
	}
}



void FiringSolutionCalculator::calculateFiringSolutions()
{
	//calculate a solution for every "maneuvering group". After every group there is a check whether the calculation
	//should be aborted, so it doesn't have to finish the whole solution if it is no longer needed.
	
	//assign all thrusters to groups and calculate the overall properties of the RCS in its entirety
	thrusters = new FSThrusterCollection(thrusterhandles, vessel);

	if (abort) return;
	pitchUpSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_PITCHUP));
	if (abort) return;
	pitchDownSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_PITCHDOWN));
	if (abort) return;
	yawRightSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_YAWRIGHT));
	if (abort) return;
	yawLeftSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_YAWLEFT));
	if (abort) return;
	bankRightSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_BANKRIGHT));
	if (abort) return;
	bankLeftSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_BANKLEFT));
	if (abort) return;
	leftSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_LEFT));
	if (abort) return;
	rightSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_RIGHT));
	if (abort) return;
	upSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_UP));
	if (abort) return;
	downSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_DOWN));
	if (abort) return;
	forwardSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_FORWARD));
	if (abort) return;
	backSol = completeFiringSolution(calculateFiringSolution(THGROUP_ATT_BACK));

	solutionready = true;
}


FIRING_SOLUTION FiringSolutionCalculator::constructFiringSolution(VECTOR3 &desiredForce, VECTOR3 &desiredTorque)
{
	//set this so the solution has the right thruster handles so it can add, but zero coefficents
	FIRING_SOLUTION result = (pitchUpSol * 0);
	result += ((desiredForce.x > 0) ? rightSol : leftSol) * abs(desiredForce.x);
	result += ((desiredForce.y > 0) ? upSol : downSol) * abs(desiredForce.y);
	result += ((desiredForce.z > 0) ? forwardSol : backSol) * abs(desiredForce.z);
	result += ((desiredTorque.x > 0) ? pitchUpSol : pitchDownSol) * abs(desiredTorque.x);
	result += ((desiredTorque.y > 0) ? yawRightSol : yawLeftSol) * abs(desiredTorque.y);
	result += ((desiredTorque.z > 0) ? bankRightSol : bankLeftSol) * abs(desiredTorque.z);

	return result;
}



/*VECTOR3 FiringSolutionCalculator::getPrincipleForceVectorForGroup(THGROUP_TYPE group)
{
	switch (group)
	{
	case THGROUP_ATT_FORWARD:
		return _V(0, 0, 1);
	case THGROUP_ATT_BACK:
		return _V(0, 0, -1);
	case THGROUP_ATT_UP:
		return _V(0, 1, 0);
	case THGROUP_ATT_DOWN:
		return _V(0, -1, 0);
	case THGROUP_ATT_RIGHT:
		return _V(1, 0, 0);
	case THGROUP_ATT_LEFT:
		return _V(0, 0, 1);
	default:
		return _V(0, 0, 0);
	}
}*/


/*VECTOR3 FiringSolutionCalculator::getPrincipleTorqueVectorForGroup(THGROUP_TYPE group)
{
	switch (group)
	{
	case THGROUP_ATT_BANKRIGHT:
		return _V(0, 0, 1);
	case THGROUP_ATT_BANKLEFT:
		return _V(0, 0, -1);
	case THGROUP_ATT_YAWLEFT:
		return _V(0, 1, 0);
	case THGROUP_ATT_YAWRIGHT:
		return _V(0, -1, 0);
	case THGROUP_ATT_PITCHUP:
		return _V(1, 0, 0);
	case THGROUP_ATT_PITCHDOWN:
		return _V(0, 0, 1);
	default:
		return _V(0, 0, 0);
	}
}*/


vector<THGROUP_TYPE> FiringSolutionCalculator::getGroupsFromForceVector(VECTOR3 &force, FORCETYPE type)
{
	vector<THGROUP_TYPE> groups;
	if (force.x > 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_RIGHT);
		}
		else
		{
			groups.push_back(THGROUP_ATT_PITCHDOWN);
		}
	}
	else if (force.x < 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_LEFT);
		}
		else
		{
			groups.push_back(THGROUP_ATT_PITCHUP);
		}
	}
	
	if (force.y > 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_UP);
		}
		else
		{
			groups.push_back(THGROUP_ATT_YAWRIGHT);
		}
	}
	else if (force.y < 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_DOWN);
		}
		else
		{
			groups.push_back(THGROUP_ATT_YAWLEFT);
		}
	}
	if (force.z > 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_FORWARD);
		}
		else
		{
			groups.push_back(THGROUP_ATT_BANKLEFT);
		}
	}
	else if (force.z < 0.0)
	{
		if (type == F_LINEAR)
		{
			groups.push_back(THGROUP_ATT_BACK);
		}
		else
		{
			groups.push_back(THGROUP_ATT_BANKRIGHT);
		}
	}
	return groups;
}


FIRING_SOLUTION FiringSolutionCalculator::calculateFiringSolution(THGROUP_TYPE group)
{
	//figure out if this thruster group is for translation or rotation
	FORCETYPE forcetype, undesired_forcetype;
	if ((int)group <= 8)
	{
		forcetype = F_TORQUE;
		undesired_forcetype = F_LINEAR;
	}
	else
	{
		forcetype = F_LINEAR;
		undesired_forcetype = F_TORQUE;
	}

	//get all thrusters in the group to be engaged
	vector<FiringSolutionThruster*> groupthrusters = thrusters->GetThrustersInGroup(group);

	if (groupthrusters.size() == 0)
	{
		//there aren't any thrusters in this group, there's no solution possible!
		return FIRING_SOLUTION();
	}

	//initialise firing solution with all involved thrusters to a coefficient of 1
	FIRING_SOLUTION result;
	for (unsigned int i = 0; i < groupthrusters.size(); ++i)
	{
		result.coefficents[groupthrusters[i]->GetHandle()] = 1;
	}

	//get the overall forces produced by the engaged thrusters
	map<FORCETYPE, VECTOR3> generatedforces = calculateGeneratedForce(result, groupthrusters);


	
	//loop until we managed to eliminate undesired forces, or decide to give up
	VECTOR3 undesired_force = generatedforces[undesired_forcetype];
	if (Calc::IsNear(undesired_force, _V(0, 0, 0), 0.001))
	{
		//no further calculation needed, there's no unwanted force induced
		return result;
	}

	//get the thrustergroups contributing to the unwanted force
	vector<THGROUP_TYPE> checkgroups = getGroupsFromForceVector(undesired_force, undesired_forcetype);
	//get all currently used thrusters contributing to the unwanted force, sorted by their suitability for the group we actually want.
	vector<FiringSolutionThruster*> checkthrusters = thrusters->GetGroupUnion(checkgroups, group);

	bool abortcalculation = false;
	int iterations = 0;
	double scorekeeper = -1;					//the current score to choose thrusters
	map<FORCETYPE, VECTOR3> currentforce;

	while (!Calc::IsNear(undesired_force, _V(0, 0, 0), 0.001) && iterations < 20)
	{
		//first choose the thrusters we're going to manipulate in this iteration
		vector<FiringSolutionThruster*> chosenthrusters;

		//walk through the offending thrusters backwards (least suited for what we actually want go first)
		//we'll group thrusters with equal scores together, as they have a very good chance of being symmetrical.
		for (UINT i = checkthrusters.size(); i > 0; --i)
		{
			int idx = i - 1;
			//get the current coefficient of this thruster
			auto curcoef = result.coefficents.find(checkthrusters[idx]->GetHandle());
			if (curcoef->second < 0.1)
			{
				//this thruster might as well not fire at all
				curcoef->second = 0.0;
				checkthrusters.erase(checkthrusters.begin() + (idx));
			}
			else
			{
				double groupscore = checkthrusters[idx]->GetScore(group);
				if (groupscore >= scorekeeper)
				{
					if (scorekeeper == -1)
					{
						//this is the first thruster in the first iteration, so we'll start with this one
						scorekeeper = groupscore;
						chosenthrusters.push_back(checkthrusters[idx]);
					}
					else if (scorekeeper == groupscore)
					{
						//all thrusters with this score should be evaluated in this iteration, so add it.
						chosenthrusters.push_back(checkthrusters[idx]);
					}
					else
					{
						//we've left the region of thrusters with eligible scores, stop looking.
						//continue with thrusters with better scores in the next iteration, to see if that helps.
						scorekeeper = groupscore;
						break;
					}
				}
			}

			//if we've been through all thrusters, start from the lowest score again to see if further iteration brings any improvements.
			if (idx = 0)
			{
				scorekeeper = -1;
			}
		}

		//if there's no thrusters left to choose from, we did all we can. There's just no clean solution to this one.
		if (chosenthrusters.size() == 0)
		{
			break;
		}
		
		//this leaves us with a number of thrusters that are least suited for the purpose we want, but contribute to unwanted force. yay!
		//let's see how much force they actually generate!
		auto subforce = calculateGeneratedForce(result, chosenthrusters);
		VECTOR3 undesired_subforce = subforce[undesired_forcetype];

		//now it's getting abstract. The dotproduct of the force we produce and the force we want to get rid of
		//is a classifier for "how much" of the vector we produce actually contributes to the undesired force.
		double dotprod = dotp(undesired_subforce, undesired_force);
		//the dotproduct of the force we produce is nothing but the squared magnitude of the total force we produce.
		double subforceprod = dotp(undesired_subforce, undesired_subforce);
		//dividing "how much" we contribute by the squared magnitude of the contributor gives us the actual fraction of how much of our magnitude is contributing
		//to the unwanted force. 1 minus that fraction is the factor by which we have to multiply the produced force to cancel out as much as possible of the
		//undesired force while losing as little as possible of desired force. If the result is negative, that means we'd have to actually produce a counter-force
		//to the producing vector to cancel out all unwanted force, so we just shut down the thrusters in question and continue in the algorithm.
		
		double newcoefficient = max(0, 1 - (dotprod / subforceprod));
		for (UINT i = 0; i < chosenthrusters.size(); ++i)
		{
			result.coefficents[chosenthrusters[i]->GetHandle()] = newcoefficient;
		}

		//recalculate the generated force, and how much excess we're having.
		currentforce = calculateGeneratedForce(result, groupthrusters);
		undesired_force = currentforce[undesired_forcetype];
		iterations++;
	}
	//note the maximum forces for each group, as absolute force (i.e. this is a vector that's used as a collection with three items).
	currentforce[F_TORQUE].x = abs(currentforce[F_TORQUE].x);
	currentforce[F_TORQUE].y = abs(currentforce[F_TORQUE].y);
	currentforce[F_TORQUE].z = abs(currentforce[F_TORQUE].z);
	currentforce[F_LINEAR].x = abs(currentforce[F_LINEAR].x);
	currentforce[F_LINEAR].y = abs(currentforce[F_LINEAR].y);
	currentforce[F_LINEAR].z = abs(currentforce[F_LINEAR].z);
	maxforces[group] = currentforce;
	return result;
}


map<FORCETYPE, VECTOR3> FiringSolutionCalculator::calculateGeneratedForce(FIRING_SOLUTION &sol, vector<FiringSolutionThruster*> &involvedthrusters)
{
	VECTOR3 force = _V(0, 0, 0);
	VECTOR3 torque = _V(0, 0, 0);
	//walk through all thrusters in the group and add their vectors
	for (UINT i = 0; i < involvedthrusters.size(); ++i)
	{
		force += involvedthrusters[i]->GetLinearForce() * sol.coefficents[involvedthrusters[i]->GetHandle()];
		torque += involvedthrusters[i]->GetTorque() * sol.coefficents[involvedthrusters[i]->GetHandle()];
	}
	//note the resulting vectors in a map
	map<FORCETYPE, VECTOR3> totalforces;
		
	totalforces[F_LINEAR] = force;
	totalforces[F_TORQUE] = torque;
	return totalforces;
}


FIRING_SOLUTION FiringSolutionCalculator::completeFiringSolution(FIRING_SOLUTION &sol)
{
	FIRING_SOLUTION finalsolution;
	for (UINT i = 0; i < thrusterhandles.size(); ++i)
	{
		finalsolution.coefficents[thrusterhandles[i]] = 0.0;
	}
	for (auto it = sol.coefficents.begin(); it != sol.coefficents.end(); ++it)
	{
		finalsolution.coefficents[it->first] = it->second;
	}
	return finalsolution;
}


