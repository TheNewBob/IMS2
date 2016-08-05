#include "GUIincludes.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "ModuleFunctionIncludes.h"
//#include "IMS_ModuleFunction_Pressurised.h"
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

	currenttimewarp = oapiGetTimeAcceleration();
	scaleInjectorResource();
	v->SetPropellantEfficiency(injector, efficiency);
}

IMS_PropellantInjector::~IMS_PropellantInjector()
{

}


bool IMS_PropellantInjector::ConnectTank(IMS_Storable* tank)
{
	//Is the tanks propellant type compatible with this injector?
	if (ratio.find(tank->GetConsumableId()) != ratio.end())
	{
		//is the tank not yet registered with this injector?
		if (find(tanks.begin(), tanks.end(), tank) == tanks.end())
		{
			//everything checks out, register the tank
			tanks.push_back(tank);
			checkValidity();
			return true;
		}
	}
	return false;
}


bool IMS_PropellantInjector::DisconnectTank(IMS_Storable* tank)
{
	//Is the tanks propellant type compatible with this injector?
	if (ratio.find(tank->GetConsumableId()) != ratio.end())
	{
		//search for the tank and remove it if it was registered
		vector<IMS_Storable*>::iterator i = find(tanks.begin(), tanks.end(), tank);
		if (i != tanks.end())
		{
			tanks.erase(i);
			checkValidity();
			return true;
		}
	}
	return false;
}


bool IMS_PropellantInjector::PreStep()
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

	//the mass consumed from the resource is the total mass of propellant consumed by thrusters connected to this injector in this frame.
	double curmass = v->GetPropellantMass(injector);
	double consumedmass = injectormass - curmass;

	//check if the timewarp has changed, and resize the virtual resource accordingly
	double newtimewarp = oapiGetTimeAcceleration();
	if (newtimewarp != currenttimewarp)
	{
		currenttimewarp = newtimewarp;
		scaleInjectorResource();
	}


	//the reactionary way in which propellant consumption is handled is inherently inprecise.
	//when a tank runs empty, it will always provide more propellant in its last gasp than it actually has, 
	//because that propellant has already been consumed by orbiter. The result can be significant inprecisions
	//in applied Delta-V on the last frame a thruster is running when the engine has a high massflow rate and
	//time acceleration is high. However, both of these conditions happening at once are somewhat unlikely,
	//since engines with high massflow are not likely to be used at high time accel.

	if (consumedmass > 0.0)
	{


		vector<IMS_Storable*> empty_tanks;						//will contain any tanks that ran dry during this frame
		for (UINT i = 0; i < tanks.size(); ++i)
		{
			int consumableid = tanks[i]->GetConsumableId();
			//calculate how much of the mass has to be consumed from this tank
			double mass_from_tank = consumedmass * ratio[consumableid];

			//consume the mass from the tank and check if the necessary amount could be consumed
			double removedcontent = tanks[i]->RemoveContent(mass_from_tank, consumableid);
			if (removedcontent < mass_from_tank || tanks[i]->GetMass() < 0.001)
			{
				//the tank is empty, note it for disconnection
				empty_tanks.push_back(tanks[i]);

				//debug - write the overtaxed amount to the log for testing.
				stringstream ss;
				ss << "consumed " << mass_from_tank - removedcontent << " too much from tank.";
				Helpers::writeToLog(ss.str(), L_DEBUG);
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
		v->SetPropellantMass(injector, injectormass);
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
	for (UINT i = 0; i < tanks.size(); ++i)
	{
		present_prop_types.insert(tanks[i]->GetConsumableId());
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
	scaleInjectorResource();
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
		scaleInjectorResource();
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

	//rescale the virtual propellant source
	scaleInjectorResource();
	injectormasschanged = true;
	//Rescaling the resource will make it larger, but it will not be filled up entirely.
	//the missing amount will be interpreted as propellant consumption in the next prestep,
	//which is not appropriate in this case, so we have to fill it up.
	v->SetPropellantMass(injector, injectormass);
}


void IMS_PropellantInjector::scaleInjectorResource()
{
	double previousinjectormass = injectormass;
	//scales the virtual propellant resource to contain enough mass for two frames at the current framerate, to a maximum of the ammount of propellant available to the injector
	double maxmass = GetAvailablePropellantMass();
	//calculate how many in-universe seconds currently pass per frame
	double framerate = oapiGetFrameRate();
	//at loadup, the framerate will be zero. We assume a low framerate to be on the save side.
	if (framerate == 0.0)
	{
		framerate = 15;
	}
	double frameduration = 1.0 / framerate * currenttimewarp;
	//calculate how much mass is needed to sustain all connected thrusters for two frames
	double massfortwoframes = totalmaxmassflow * frameduration * 2;
	//set the injector mass, and resize the propellant resource
	injectormass = min(massfortwoframes, maxmass);
	if (injector == NULL)
	{
		//the resource hasn't even been created yet.
		injector = v->CreatePropellantResource(injectormass, injectormass);
	}
	else
	{	//check how much propellant is missing from the resource at this time.
		//At the end of the operation, the same ammount has to be missing as at the beginning,
		//or propellant will go missing from the tanks with every rescale!
		double missingprop = previousinjectormass - v->GetPropellantMass(injector);
		
		double newcurrentmass = injectormass - missingprop;
		if (newcurrentmass < 0)
		{
			//When downscaling, it can happen that we cannot adequately represent what was missing from the resource.
			//For now we'll log it to get an idea how often this happens and how much error is introduced by it.
			Helpers::writeToLog(string("Propellant consumption ignored due to downscaling of virtual resource: " + Helpers::doubleToString(newcurrentmass * -1)), L_DEBUG);
			newcurrentmass = 0;
		}
		v->SetPropellantMaxMass(injector, injectormass);
		v->SetPropellantMass(injector, newcurrentmass);
	}
	
	//notify that the mass of the virtual resource has changed
	injectormasschanged = true;
}

double IMS_PropellantInjector::GetAvailablePropellantMass()
{
	double availablemass = 0.0;
	for (UINT i = 0; i < tanks.size(); ++i)
	{
		availablemass += tanks[i]->GetMass();
	}
	return availablemass;
}
