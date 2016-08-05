#include "Common.h"
#include "Events.h"
#include "CoGHasChangedEvent.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "IMS_CoGmanager.h"
#include "calc.h"




IMS_CoGmanager::IMS_CoGmanager(IMS2 *_vessel) : IMS_Manager_Base(_vessel), vessel(_vessel), cogpos(_V(0, 0, 0))
{
}


IMS_CoGmanager::~IMS_CoGmanager()
{
}

void IMS_CoGmanager::SetCoG()
{
	/* if the vessel is landed, don't shift the CG, return NULL (don't worry, the event generator will filter it out)*/
	if (vessel->GetLandedState())
	{
		return;
	}
	
	VECTOR3 oldcg = cogpos;
	//reset the cogpos to its original position
	vessel->ShiftCG(cogpos * -1);
	//calculate the new cog
	cogpos = calculateNewCoG();
	//set the cog to the new position
	vessel->ShiftCG(cogpos);

	SetMass();

	addEvent(new CoGHasChangedEvent(oldcg, cogpos));
}

//TODO enable to pass already calculated mass of modules for optimisation
void IMS_CoGmanager::SetMass()
{
	double mass = 0;
	vector<IMS_Module*> modules;
	vessel->GetModules(modules);

	for (UINT i = 0; i < modules.size(); ++i)
	{
		mass += modules[i]->GetMass();
	}


	//subtract the propellant mass, since in this case this is purely virtual,
	//only there for orbiters thrusters benefit. The actual propellant mass 
	//is contained in the Storables.
	mass -= vessel->GetTotalPropellantMass();

	vessel->SetEmptyMass(mass);
}


void IMS_CoGmanager::InitialiseCoG(VECTOR3 cog)
{
	cogpos = cog;
	vessel->ShiftCG(cogpos);
}


VECTOR3 IMS_CoGmanager::calculateNewCoG()
{
	
	//get all modules from the vessel
	vector<IMS_Module*> modules;
	vessel->GetModules(modules);

	VECTOR3 newCoG = _V(0, 0, 0);
	double accumulatedMass = modules[0]->GetMass();

	for (UINT i = 1; i < modules.size(); ++i)
	{
		VECTOR3 modulepos;
		modules[i]->GetPos(modulepos);
		double modulemass = modules[i]->GetMass();
		newCoG = ((newCoG * accumulatedMass) + (modulepos * modulemass)) / (accumulatedMass + modulemass);
		accumulatedMass += modulemass;
	}
	Calc::RoundVector(newCoG, 1000);

	return newCoG;
}


vector<IMS_Module*> IMS_CoGmanager::removeSymmetricModules(vector<IMS_Module*> &symmetricallysortedmodules)
{
	vector<IMS_Module*>returnlist;
	for (UINT i = 1; i < symmetricallysortedmodules.size(); ++i)
	{
		IMS_Module *module1 = symmetricallysortedmodules[i - 1];
		IMS_Module *module2 = symmetricallysortedmodules[i];
		//compare if the two modules are symmetric
		VECTOR3 module1pos, module2pos;
		module1->GetPos(module1pos);
		module2->GetPos(module2pos);
		bool modulesaresymmetrical = false;
		if (Calc::IsEqual(module1pos.z, module2pos.z))
		{
			//the modules are located on the same position along the z axis, so they might be symmetrical
			//if they are in truly symmetric locations, the x and y positions will equal out to zero when added.
			VECTOR3 symmetryproof = module1pos + module2pos;
			if (Calc::IsEqual(symmetryproof.x, 0) && Calc::IsEqual(symmetryproof.y, 0))
			{
				//the modules are symmetric, but do they have the same mass?
				if (Calc::IsEqual(module1->GetMass(), module2->GetMass()))
				{
					//the modules are symmetrical in position and mass!
					modulesaresymmetrical = true;
					//increment i, so the module at current i will not be compared to during the next iteration
					i++;
				}
			}
			
		}
		
		if (!modulesaresymmetrical)
		{
			//the modules are not symmetrical, add themodule at i -1 to the list. 
			//The other one might still turn out to be symmetric with the one at i + 1
			returnlist.push_back(symmetricallysortedmodules[i - 1]);
		}
	}
	return returnlist;
}


bool IMS_CoGmanager::ProcessEvent(Event_Base *e)
{
	if (*e == VESSELLAYOUTCHANGEDEVENT)
	{
		//the vessel layout has changed, we need to shift the center of gravity!
		addEventToWaitingQueue(new ShiftCGEvent(0));
	}

	else if (*e == SHIFTCGEVENT)
	{
		if (e->GetEventPipe() == WAITING_PIPE)
		{
			//the event has come from the waiting queue, recalculate the CoG!
			SetCoG();
		}
		else
		{
			//the event has just come in, put it on the waiting queue for processing during poststep
			addEventToWaitingQueue(new ShiftCGEvent());
		}
		return true;
	}

	else if (*e == LIFTOFFEVENT)
	{
		addEventToWaitingQueue(new ShiftCGEvent(0));
	}

	else if (*e == MASSHASCHANGEDEVENT)
	{
		//check where the event came from. If it came from outside, then add 
		//the event to the waiting queue. If it has come from the waiting queue, 
		//recalculate the mass
		if (e->GetEventPipe() != WAITING_PIPE)
		{
			addEventToWaitingQueue(new MassHasChangedEvent(1));
		}
		else
		{
			//set the timer for CoG-recalculation, but set the new mass right now.
			addEvent(new ShiftCGEvent);
			SetMass();
		}
		return true;
	}
	return false;
}