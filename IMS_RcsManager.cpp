#include "common.h"
#include "events.h"
#include "IMS.h"
#include <thread>
#include "FiringSolutionThruster.h"
#include "FSThrusterCollection.h"
#include "FiringSolutionCalculator.h"
#include "IMS_Manager_Base.h"
#include "IMS_RcsManager.h"
#include "Calc.h"
#include "IMS_Matrix.h"




IMS_RcsManager::IMS_RcsManager(IMS2 *_vessel)
	:	IMS_Manager_Base(_vessel)
{
	//create propellant resource for dummy thrusters. 1 kg should be enough.
	dummypropellant = vessel->CreatePropellantResource(100);
}


IMS_RcsManager::~IMS_RcsManager()
{
	destroyFiringSolution();
}


void IMS_RcsManager::AddThruster(THRUSTER_HANDLE thruster)
{
	assert(find(rcsthrusters.begin(), rcsthrusters.end(), thruster) == rcsthrusters.end()
		&& "Attempting to add thruster twice to RcsManager!");
	//if the dummy thrusters don't exist yet, create them now
	if (!dummiesexist && intelligentrcs)
	{
		createDummyThrusters();
	}

	rcsthrusters.push_back(thruster);
	
	//add an event that will fire in post-step
	addEventToWaitingQueue(new RcsChangedEvent);
}


void IMS_RcsManager::AddThrusterPair(THRUSTER_HANDLE thruster1, THRUSTER_HANDLE thruster2)
{
	assert(find(rcsthrusters.begin(), rcsthrusters.end(), thruster1) == rcsthrusters.end()
		&& "Attempting to add thruster twice to RcsManager!");
	assert(find(rcsthrusters.begin(), rcsthrusters.end(), thruster2) == rcsthrusters.end()
		&& "Attempting to add thruster twice to RcsManager!");

	//if the dummy thrusters don't exist yet, create them now
	if (!dummiesexist && intelligentrcs)
	{
		createDummyThrusters();
	}

	rcsthrusters.push_back(thruster1);
	rcsthrusters.push_back(thruster2);

	//add an event that will fire in post-step
	addEventToWaitingQueue(new RcsChangedEvent);
}


void IMS_RcsManager::RemoveThruster(THRUSTER_HANDLE thruster)
{
	assert(find(rcsthrusters.begin(), rcsthrusters.end(), thruster) != rcsthrusters.end()
		&& "Attempting to remove thruster from RcsManager that was never added!");
	//if there is a calculation running, abort it. The thread will crash if a thruster is deleted
	//from the vessel while it's running. 
	if (newfiringsolution != NULL)
	{
		delete newfiringsolution;
		newfiringsolution = NULL;
	}
	//if there is an active firing solution, it uses thrusters that are no longer a part of this vessel.
	if (firingsolution != NULL)
	{
		delete firingsolution;
		firingsolution = NULL;
	}
	
	//remove the thruster from the list
	auto vecit = find(rcsthrusters.begin(), rcsthrusters.end(), thruster);
	rcsthrusters.erase(vecit);

	//if there are no rcs thrusters left on the vessel, delete the dummy thrusters.
	//otherwise people will still hear the sounds of rcs firing when they hit the controlls
	if (rcsthrusters.size() == 0)
	{
		destroyDummyThrusters();
	}

	addEventToWaitingQueue(new RcsChangedEvent);
}

void IMS_RcsManager::SetIntelligentRcs(bool enabled)
{
	if (enabled != intelligentrcs)
	{
		if (enabled)
		{
			//intelligent rcs was disabled and should now beenabled.
			intelligentrcs = true;
			destroyPhysicalRcsGroups();
			createDummyThrusters();
			calculateFiringSolution();
		}
		else
		{
			//intelligent rcs was enabled and should now be disabled
			if (firingsolution != NULL && firingsolution->IsSolutionReady())
			{
				//we have to shut down all the thrusters, or they might keep running
				firingsolution->Apply(_V(0, 0, 0), _V(0, 0, 0), 1);
			}
			intelligentrcs = false;
			destroyDummyThrusters();
			destroyFiringSolution();
			createPhysicalRcsGroups();
		}
	}
}

void IMS_RcsManager::SetCommandedTorque(VECTOR3 &torque)
{
	//for now we'll work on a "last past the post" system.
	commandedtorque = torque;
	torqueset = true;
}


void IMS_RcsManager::createPhysicalRcsGroups()
{
	if (rcsthrusters.size() > 0)
	{
		//assign all thrusters to groups and calculate the overall properties of the RCS in its entirety
		FSThrusterCollection thrusters(rcsthrusters, vessel);
		//now assign all rcs thrusters to their appropriate groups
		for (int i = 3; i < 15; ++i)	//iterate through relevant groups in THGROUP_TYPE enum
		{
			//get all thrusters in the current group and shove them into an array to pass them to orbiter
			vector<FiringSolutionThruster*> groupthrusters = thrusters.GetThrustersInGroup((THGROUP_TYPE)i);

			THRUSTER_HANDLE *tharray = new THRUSTER_HANDLE[groupthrusters.size()];
			for (UINT j = 0; j < groupthrusters.size(); ++j)
			{
				tharray[j] = groupthrusters[j]->GetHandle();
			}
			vessel->CreateThrusterGroup(tharray, groupthrusters.size(), (THGROUP_TYPE)i);
			delete[] tharray;
		}
	}
}


void IMS_RcsManager::destroyPhysicalRcsGroups()
{
	if (!dummiesexist)
	{
		//delete the current thruster groups, but keep the thrusters!
		for (int i = 3; i < 15; ++i)
		{
			vessel->DelThrusterGroup((THGROUP_TYPE)i, false);
		}
	}
}


void IMS_RcsManager::destroyFiringSolution()
{
	if (newfiringsolution != NULL)
	{
		delete newfiringsolution;
		newfiringsolution = NULL;
	}
	if (firingsolution != NULL)
	{
		delete firingsolution;
		firingsolution = NULL;
	}
}

void IMS_RcsManager::PreStep(double simdt)
{
	if (intelligentrcs)
	{
		//check if a new solution is being calculated
		if (newfiringsolution != NULL)
		{
			//check if the calculation has finished
			if (newfiringsolution->IsSolutionReady())
			{
				//delete the current solution, set the used solution to the new one
				delete firingsolution;
				firingsolution = newfiringsolution;
				newfiringsolution = NULL;
			}
		}
		//check if we have RCS, and a solution
		if (firingsolution != NULL && dummiesexist)
		{
			//calculate force and torque to be applied to the vessel
			VECTOR3 force;
			VECTOR3 torque;
			getCommandedForce(force);
			getCommandedTorque(torque);
	
			//check if the user is currently requesting any thrust.
			bool nothrustrequest = Calc::IsEqual(force, _V(0, 0, 0)) &&
								   Calc::IsEqual(torque, _V(0, 0, 0));
								   
			if (nothrustrequest && torqueset)
			{
				//there might be no request from the user, but an autopilot wants some.
				nothrustrequest = false;
				torque = commandedtorque;
				firingsolution->CalculateTorqueLevels(torque);
			}
			if (torqueset) torqueset = false;


			//Manipulate the thrusters if there is a user request to do so, or if they are currently firing
			if (!nothrustrequest || thrustersfiring)
			{
				if (nothrustrequest)
				{
					//there is no request from the user, but the thrusters are currently firing. 
					//Meaning they need to stop firing!
					thrustersfiring = false;
				}
				else if (!nothrustrequest && !thrustersfiring)
				{
					//there's a thrust request, but the thrusters are not currently firing. They need to get going!
					thrustersfiring = true;
				}
				//in either case, we have to apply the firing solution. It will either start or stop the thrusters.
				firingsolution->Apply(torque, force, 1.0);
				//refill the dummys propellant so it won't run dry
				vessel->SetPropellantMass(dummypropellant, vessel->GetPropellantMaxMass(dummypropellant));
			}
		}
	}
}


bool IMS_RcsManager::ProcessEvent(Event_Base *e)
{
	if (*e == CGHASCHANGEDEVENT)
	{
		addEventToWaitingQueue(new RcsChangedEvent);
	}
	else if (*e == RCSCHANGEDEVENT)
	{
		//this event comes from the waiting queue, ergo we are now in post-step.
		if (intelligentrcs)
		{
			calculateFiringSolution();
		}
		else
		{
			destroyPhysicalRcsGroups();
			createPhysicalRcsGroups();
		}
	}
	else if (*e == SIMULATIONSTARTEDEVENT)
	{
		//fill up the virtual propellant resource - there's a chance orbiter emptied it between PostCreation and SimStart!
		vessel->SetPropellantMass(dummypropellant, vessel->GetPropellantMaxMass(dummypropellant));
	}
	return false;
}


void IMS_RcsManager::calculateFiringSolution()
{
	if (newfiringsolution != NULL)
	{
		//oops, a new calculation is requested before the last one finished.
		//terminate the running thread at the next opportunity, and then get on with it.
		//newfiringsolution->AbortCalculation();
		delete newfiringsolution;
	}
	newfiringsolution = new FiringSolutionCalculator(vessel, rcsthrusters);
}


void IMS_RcsManager::getCommandedForce(VECTOR3 &OUT_force)
{
	double leftLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[12]));  //THGROUP_ATT_LEFT
	double rightLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[14]));  //THGROUP_ATT_RIGHT
	double upLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[16]));  //THGROUP_ATT_UP
	double downLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[18]));  //THGROUP_ATT_DOWN
	double forwardLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[20]));  //THGROUP_ATT_FORWARD
	double backLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[22]));  //THGROUP_ATT_BACK
	OUT_force = _V(leftLevel - rightLevel, upLevel - downLevel, forwardLevel - backLevel);
}


void IMS_RcsManager::getCommandedTorque(VECTOR3 &OUT_torque)
{
	//user input takes precedence
	double pitchUpLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[0])); //THGROUP_ATT_PITCHUP
	double pitchDownLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[2])); //THGROUP_ATT_PITCHDOWN
	double yawRightLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[4])); //THGROUP_ATT_YAWRIGHT
	double yawLeftLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[6])); //THGROUP_ATT_YAWLEFT
	double bankRightLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[8])); //THGROUP_ATT_BANKRIGHT
	double bankLeftLevel = Helpers::fixDoubleNaN(vessel->GetThrusterLevel(dummyThrusters[10])); //THGROUP_ATT_BANKLEFT
	OUT_torque = _V(pitchUpLevel - pitchDownLevel, yawLeftLevel - yawRightLevel, bankRightLevel - bankLeftLevel);
}


void IMS_RcsManager::createDummyThrusters()
{
	//create dummy thrusters on the vessel so the thruster group levels can be read correctly.
	//these thrusters have negligible thrust, so they shouldn't influence the result to any measurable amount.
	//they also don't consume any propellant to speak of, so we can do with a virtual propellant resource of 1 kg.
	//all this is necessary simply to read the user input, because orbiter doesn't process that otherwise.

	double rcsthrust = 1e-12;
	double rcsisp = 1e12;
	dummyThrusters[0] = vessel->CreateThruster(_V(0, 0, 0), _V(1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[1] = vessel->CreateThruster(_V(0, 0, 0), _V(-1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters, 2, THGROUP_ATT_PITCHUP);
	dummyThrusters[2] = vessel->CreateThruster(_V(0, 0, 0), _V(-1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[3] = vessel->CreateThruster(_V(0, 0, 0), _V(1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 2, 2, THGROUP_ATT_PITCHDOWN);
	dummyThrusters[4] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 1, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[5] = vessel->CreateThruster(_V(0, 0, 0), _V(0, -1, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 4, 2, THGROUP_ATT_YAWRIGHT);
	dummyThrusters[6] = vessel->CreateThruster(_V(0, 0, 0), _V(0, -1, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[7] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 1, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 6, 2, THGROUP_ATT_YAWLEFT);
	dummyThrusters[8] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, 1), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[9] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, -1), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 8, 2, THGROUP_ATT_BANKRIGHT);
	dummyThrusters[10] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, -1), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[11] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, 1), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 10, 2, THGROUP_ATT_BANKLEFT);
	dummyThrusters[12] = vessel->CreateThruster(_V(0, 0, 0), _V(1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[13] = vessel->CreateThruster(_V(0, 0, 0), _V(-1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 12, 2, THGROUP_ATT_LEFT);
	dummyThrusters[14] = vessel->CreateThruster(_V(0, 0, 0), _V(-1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[15] = vessel->CreateThruster(_V(0, 0, 0), _V(1, 0, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 14, 2, THGROUP_ATT_RIGHT);
	dummyThrusters[16] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 1, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[17] = vessel->CreateThruster(_V(0, 0, 0), _V(0, -1, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 16, 2, THGROUP_ATT_UP);
	dummyThrusters[18] = vessel->CreateThruster(_V(0, 0, 0), _V(0, -1, 0), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[19] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 1, 0), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 18, 2, THGROUP_ATT_DOWN);
	dummyThrusters[20] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, 1), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[21] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, -1), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 20, 2, THGROUP_ATT_FORWARD);
	dummyThrusters[22] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, -1), rcsthrust, dummypropellant, rcsisp);
	dummyThrusters[23] = vessel->CreateThruster(_V(0, 0, 0), _V(0, 0, 1), rcsthrust, dummypropellant, rcsisp);
	vessel->CreateThrusterGroup(dummyThrusters + 22, 2, THGROUP_ATT_BACK);

	dummiesexist = true;
}


void IMS_RcsManager::destroyDummyThrusters()
{
	//destroy all the dummy thrusters
	vessel->DelThrusterGroup(THGROUP_ATT_PITCHUP, true);
	vessel->DelThrusterGroup(THGROUP_ATT_PITCHDOWN, true);
	vessel->DelThrusterGroup(THGROUP_ATT_YAWLEFT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_YAWRIGHT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_BANKLEFT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_BANKRIGHT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_LEFT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_RIGHT, true);
	vessel->DelThrusterGroup(THGROUP_ATT_UP, true);
	vessel->DelThrusterGroup(THGROUP_ATT_DOWN, true);
	vessel->DelThrusterGroup(THGROUP_ATT_FORWARD, true);
	vessel->DelThrusterGroup(THGROUP_ATT_BACK, true);

	dummiesexist = false;
}


