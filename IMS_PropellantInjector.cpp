#include "GUIincludes.h"
#include "Common.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_Movable.h"
#include "IMS_Storable.h"
#include "IMS_PropellantInjector.h"
#include <set>



IMS_PropellantInjector::IMS_PropellantInjector(vector<int> &propellant_ids, vector<float> &ratio, double efficiency, VESSEL* vessel) : v(vessel)
{
	//both vectors must contain an equal amount of elements!
	assert(propellant_ids.size() == ratio.size());

	//calculate the sum of ratios in order to calculate the ratios as fractions of 1 further down.
	float ratio_sum = 0;
	for (UINT i = 0; i < ratio.size(); ++i)
	{
		ratio_sum += ratio[i];
	}
	double one_part = 1.0 / ratio_sum;

	for (UINT i = 0; i < propellant_ids.size(); ++i)
	{
		//store ratios as fractions of 1
		this->ratio[propellant_ids[i]] = ratio[i] * one_part;
	}

	injector = v->CreatePropellantResource(injectormass);
	v->SetPropellantEfficiency(injector, efficiency);
}

IMS_PropellantInjector::~IMS_PropellantInjector()
{

}


bool IMS_PropellantInjector::ConnectTank(IMS_Storable* tank)
{
	//Is the tanks propellant type compatible with this injector, and is there actually something in it?
	if (ratio.find(tank->GetConsumableId()) != ratio.end() && tank->GetMass() > 0.0)
	{
		//see if we have this propellant type present
		auto proptype = tanks.find(tank->GetConsumableId());
		//if we don't have any tanks of this type, or not this tank in particular, register it.
		if (proptype == tanks.end() || 
			find(proptype->second.begin(), proptype->second.end(), tank) == proptype->second.end())
		{
			tanks[tank->GetConsumableId()].push_back(tank);
			checkValidity();
			return true;
		}
	}
	return false;
}


bool IMS_PropellantInjector::DisconnectTank(IMS_Storable* tank)
{
	int consumable_id = tank->GetConsumableId();
	//Is the tanks propellant type compatible with this injector?
	if (ratio.find(consumable_id) != ratio.end())
	{
		vector<IMS_Storable*> &curtanks = tanks[consumable_id];
		//search for the tank and remove it if it was registered
		vector<IMS_Storable*>::iterator i = find(curtanks.begin(), curtanks.end(), tank);
		if (i != curtanks.end())
		{
			curtanks.erase(i);
			if (curtanks.size() == 0)
			{
				tanks.erase(tanks.find(consumable_id));
			}
			checkValidity();
			return true;
		}
	}
	return false;
}


bool IMS_PropellantInjector::PreStep(double simdt)
{
	//if the injector doesn't have the necessary propellant to work, see if the injector mass has changed due to other circumstances
	if (!isViable)
	{
		if (injectormasschanged)
		{
			injectormasschanged = false;
			return true;
		}
		return false;
	}

	
	//the mass consumed from the resource is the total mass of propellant consumed by thrusters connected to this injector in the LAST frame.
	double curmass = v->GetPropellantMass(injector);
	double consumedmass = injectormass - curmass;

	//see if any thruster levels are set
	bool is_thrusterlevel_set = areThrustersRunning();

	vector<IMS_Storable*> empty_tanks;						//will contain any tanks that ran dry during the last frame

	if (consumedmass > 0.0)
	{
		for (auto consumabletype = tanks.begin(); consumabletype != tanks.end(); ++consumabletype)
		{
			vector<IMS_Storable*> &curtanks = consumabletype->second;
			int consumableid = consumabletype->first;
			//calculate how much of the mass has to be consumed from every tank of this type
			double mass_from_tank = consumedmass * ratio[consumableid] / curtanks.size();

			for (UINT i = 0; i < curtanks.size(); ++i)
			{
				//consume the mass from the tank and check if the necessary amount could be consumed
				double removedcontent = curtanks[i]->RemoveContent(mass_from_tank, consumableid);
				if (removedcontent < mass_from_tank || curtanks[i]->GetMass() < 0.001)
				{
					//the tank is empty, note it for disconnection
					empty_tanks.push_back(curtanks[i]);
				}
			}
		}

		if (empty_tanks.size() > 0)
		{
			//some tanks ran dry during this frame, disconnect them!
			for (UINT i = 0; i < empty_tanks.size(); ++i)
			{
				DisconnectTank(empty_tanks[i]);
			}
		}

		//reset the resource to full capacity. We only need it to measure consumption
//		v->SetPropellantMass(injector, injectormass);
		if (!is_thrusterlevel_set)
		{
			//the thruster was shut down, this is the last frame that will consume propellant.
			//rescale the resource to minimum.
			scaleInjectorResource(0);
		}
	}

	//if thrusters are running, rescale the propellant resource for the next frame.
	//the cool thing is that this will be triggered an iteration
	//before anything got consumed (timestep not yet applied to sim),
	//ergo the simdt tells us for how much we need to have propellant
	//ready.
	if (is_thrusterlevel_set)
	{
		scaleInjectorResource(simdt);
	}

	if (injectormasschanged)
	{
		injectormasschanged = false;
		return true;
	}
	return false;
}


void IMS_PropellantInjector::checkValidity()
{
	//put the present consumable types into a set
	set<int> present_prop_types;
	for (auto i = tanks.begin(); i != tanks.end(); ++i)
	{
		if (i->second.size() > 0)
		{
			present_prop_types.insert(i->first);
		}
	}

	//remember the state the injector was in previously
	bool wasviable = isViable;
	isViable = true;
	for (map<int, double>::iterator i = ratio.begin(); i != ratio.end(); ++i)
	{
		if (find(present_prop_types.begin(), present_prop_types.end(), i->first) == present_prop_types.end())
		{
			//one of the needed propellant types isn't present, the injector cannot work
			disableInjector();
			isViable = false;
			break;
		}
	}

	//check if the injectors state has changed
	if (wasviable != isViable)
	{
		if (!isViable)
		{
			//disable all thrusters in the injector
			disableInjector();
		}
		else
		{
			//enable all thrusters in the injector
			enableInjector();
		}
	}
}



bool IMS_PropellantInjector::CompareMixture(vector<int> &propellant_ids, vector<float> &ratio, double efficiency)
{
	//The efficiency of the mixtures must match
	if (efficiency == v->GetPropellantEfficiency(injector))
	{
		//If the mixtures do not have the same number of propellant types, they're not identical
		if (propellant_ids.size() == this->ratio.size())
		{
			//finally, the propellant ids of the mixture must of course be the same
			for (UINT i = 0; i < propellant_ids.size(); ++i)
			{
				if (this->ratio.find(propellant_ids[i]) == this->ratio.end())
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}


void IMS_PropellantInjector::ConnectThruster(THRUSTER_HANDLE thruster, double maxmassflow)
{
	//Somebody's trying to connect the same thruster twice
	assert(thrusters.find(thruster) == thrusters.end());

	thrusters.insert(make_pair(thruster, true));
	//connect the propellant resource
	if (isViable)
	{
		v->SetThrusterResource(thruster, injector);
	}
	//update the maximum massflow of the injector
	totalmaxmassflow += maxmassflow;
}


bool IMS_PropellantInjector::DisconnectThruster(THRUSTER_HANDLE thruster, double maxmassflow)
{
	map<THRUSTER_HANDLE, bool>::iterator i = thrusters.find(thruster);
	if (i != thrusters.end())
	{
		//disconnect the propellant resource, then erase the thruster from the list
		v->SetThrusterResource(thruster, NULL);
		thrusters.erase(i);
		//update the maximum massflow of the injector. 
		totalmaxmassflow = max(0.0, totalmaxmassflow - maxmassflow);
		return true;
	}
	return false;
}

bool IMS_PropellantInjector::EnableThruster(THRUSTER_HANDLE thruster)
{
	map<THRUSTER_HANDLE, bool>::iterator i = thrusters.find(thruster);

	if (i != thrusters.end() && i->second == false)
	{
		i->second = true;
		v->SetThrusterResource(i->first, injector);
		return true;
	}
	return false;
}


bool IMS_PropellantInjector::DisableThruster(THRUSTER_HANDLE thruster)
{
	Helpers::writeToLog(string("thruster disabled!"), L_DEBUG);
	map<THRUSTER_HANDLE, bool>::iterator i = thrusters.find(thruster);

	if (i != thrusters.end() && i->second == true)
	{
		i->second = false;
		v->SetThrusterResource(i->first, NULL);
		return true;
	}
	return false;
}


void IMS_PropellantInjector::disableInjector()
{
	//walk through all thrusters and disconnect them from the propellant source, but leave their state untouched
	for (map<THRUSTER_HANDLE, bool>::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
	{
		v->SetThrusterResource(i->first, NULL);
	}

	//set the mass of the virtual resource to zero
	v->SetPropellantMaxMass(injector, 0.0);
	injectormasschanged = true;
}


void IMS_PropellantInjector::enableInjector()
{
	//walk through all thrusters and connect them to the propellant source if their operational state is ok
	for (map<THRUSTER_HANDLE, bool>::iterator i = thrusters.begin(); i != thrusters.end(); ++i)
	{
		if (i->second)
		{
			v->SetThrusterResource(i->first, injector);
		}
	}

	//rescale the virtual propellant source so we're ready to receive input again.
	scaleInjectorResource(0);
}


void IMS_PropellantInjector::scaleInjectorResource(double simdt)
{
	double previousinjectormass = injectormass;
	//scales the virtual propellant resource to contain enough mass for the next simstep,
	//or as much as we have left to burn.
	double maxmass = GetMaximumConsumablePropellantMass();
	//put a margin of 20% on what we need, just to be on the save side.
	//we'll always need something in that resource though, otherwise setting thrusterlevels
	//programmatically won't work at all.
	double massfornextframe = max(1, totalmaxmassflow * simdt * 1.1);
	
	if (maxmass < massfornextframe)
	{
		//there won't be enough propellant left to feed all thrusters at current throttle for the next frame.
		vector<THRUSTER_HANDLE> runningthrusters = getRunningThrusters();
		if (runningthrusters.size() > 1)
		{
			//there's more than one thruster running. That also means that their thrust is likely off-center.
			//We shut them all off, even though there might still be enough juice to feed them at lower thrust. Orbiter doesn't care,
			//it would just give more propellant to one of them, resulting in asymmetric thrust before shutdown.
			for (UINT i = 0; i < runningthrusters.size(); ++i)
			{
				v->SetThrusterLevel(runningthrusters[i], 0.0);
			}
			injectormass = 1;
		}
		else
		{
			//there's only one thruster running, so it's likely thrusting through the CoG. 
			//just give it all the prop that's left.
			injectormass = maxmass;
		}
	}
	else
	{
		//set a sufficient injector mass to provide enough gas for the next frame.
		injectormass = massfornextframe;
	}

	//set the new size of the resource and fill it up
	v->SetPropellantMaxMass(injector, injectormass);
	v->SetPropellantMass(injector, injectormass);
	
	
	//notify that the mass of the virtual resource has changed
	injectormasschanged = true;
}


double IMS_PropellantInjector::GetAvailablePropellantMass()
{
	double availablemass = 0.0;

	for (auto i = tanks.begin(); i != tanks.end(); ++i)
	{
		for (UINT j = 0; j < i->second.size(); ++j)
		{
			availablemass += i->second[j]->GetMass();
		}
	}
	return availablemass;
}


double IMS_PropellantInjector::GetMaximumConsumablePropellantMass()
{

	//stores the maximum total mass of mixture that could be produced from the amount
	//of the specific proptype, provided enough of the others were around.
	//e.g. imagine prop a (500kg) and b (1000kg) at a mixture of 1 to 3:
	//from prop a we could mix 2000 kg (500 + 1500) if there's enough of b available.
	//from prop b we could make 1333kg (1000 + 333).
	//in th end, all we have to do is whos value is the lowest, and we have the limiting
	//factor in the mixture as well as the maximum amount of mixture we can make.
	vector<double> maxmass_per_prop;

	for (auto i = ratio.begin(); i != ratio.end(); ++i)
	{
		//add up the total amount of propellant of this type
		vector<IMS_Storable*> &curtanks = tanks[i->first];
		double availablemass = 0.0;
		for (UINT j = 0; j < curtanks.size(); ++j)
		{
			availablemass += curtanks[j]->GetMass();
		}
		//the ratio is already converted to fractions of 1 at this point.
		maxmass_per_prop.push_back(availablemass / i->second);
	}

	//now all we have to do is sort the thing ascendingly and return the smallest value.
	sort(maxmass_per_prop.begin(), maxmass_per_prop.end());
	return maxmass_per_prop[0];
}


bool IMS_PropellantInjector::areThrustersRunning()
{
	//check the thrust level of any connected thrusters, and return true if even one of them is running.
	for (auto i = thrusters.begin(); i != thrusters.end(); ++i)
	{
		if (v->GetThrusterLevel(i->first) > 0.0) return true;
	}
	return false;
}


vector<THRUSTER_HANDLE> IMS_PropellantInjector::getRunningThrusters()
{
	vector <THRUSTER_HANDLE> runningthrusters;
	for (auto i = thrusters.begin(); i != thrusters.end(); ++i)
	{
		if (v->GetThrusterLevel(i->first) > 0)
		{
			runningthrusters.push_back(i->first);
		}
	}
	return runningthrusters;
}

void IMS_PropellantInjector::FillInjectorDummy()
{
	v->SetPropellantMass(injector, injectormass);
}