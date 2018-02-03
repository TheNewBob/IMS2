#include "common.h"
#include "events.h"
#include "IMS.h"
#include "IMS_Manager_Base.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_Movable.h"
#include "IMS_Storable.h"
#include "IMS_PropellantInjector.h"
#include "IMS_PropulsionManager.h"
#include "IMS_CoGmanager.h"

IMS_PropulsionManager::IMS_PropulsionManager(IMS2 *_vessel) : 
	IMS_Manager_Base(_vessel)
{
}


IMS_PropulsionManager::~IMS_PropulsionManager()
{
}

void IMS_PropulsionManager::AddTank(IMS_Storable *tank)
{
	//This line really only serves debugging purposes. It could be removed from release for improved performance
	map<int, vector<IMS_Storable*>>::iterator typelist = tanks.find(tank->GetConsumableId());
	//make sure the tank isn't on that list already
	if (typelist != tanks.end())
	{
		Olog::assertThat([typelist, tank]() { return find(typelist->second.begin(), typelist->second.end(), tank) == typelist->second.end(); }, "A tank was added twice to PropulsionManager!");
	}

	tanks[tank->GetConsumableId()].push_back(tank);
	addTankToInjectors(tank);
}


void IMS_PropulsionManager::RemoveTank(IMS_Storable *tank)
{
	//get a list of all tanks with this particular consumable type
	map<int, vector<IMS_Storable*>>::iterator typelist = tanks.find(tank->GetConsumableId());
	//make sure the tank is on that list 
	vector<IMS_Storable*>::iterator ti = find(typelist->second.begin(), typelist->second.end(), tank);
	//Somebody's trying to remove a tank that's never been added!
	Olog::assertThat([ti, typelist]() { return ti != typelist->second.end(); }, "Attempting to remove tank from PropulsionManager that has never been added!");

	removeTankFromInjectors(tank);
	//remove the tank from the manager
	typelist->second.erase(ti);

	if (typelist->second.size() == 0)
	{
		//there are no tanks of this consumable type left, remove the type from the map
		tanks.erase(typelist);
	}
}


void IMS_PropulsionManager::OpenTank(IMS_Storable *tank)
{
	#ifdef _DEBUG
		//make sure the tank is registered!
		//get a list of all registered tanks with this propellant type
		map<int, vector<IMS_Storable*>>::iterator typelist = tanks.find(tank->GetConsumableId());
		//find the tank on the list
		vector<IMS_Storable*>::iterator it = find(typelist->second.begin(), typelist->second.end(), tank);
		Olog::assertThat([it, typelist]() { return it != typelist->second.end(); }, "Attempting to open tank without registering it!");
	#endif

	//check if the tank is already opened
	if (!tank->IsAvailable())
	{
		tank->SetAvailable(true);
		addTankToInjectors(tank);
	}
}


void IMS_PropulsionManager::CloseTank(IMS_Storable *tank)
{
	#ifdef _DEBUG
		//make sure the tank is registered!
		//get a list of all registered tanks with this propellant type
		map<int, vector<IMS_Storable*>>::iterator typelist = tanks.find(tank->GetConsumableId());
		//find the tank on the list
		vector<IMS_Storable*>::iterator it = find(typelist->second.begin(), typelist->second.end(), tank);
		Olog::assertThat([it, typelist]() { return it != typelist->second.end(); }, "Attempting to close a tank without registering it!");
	#endif


	//check if the tank is already closed
	if (tank->IsAvailable())
	{
		tank->SetAvailable(false);
		removeTankFromInjectors(tank);
	}
}


IMS_PropellantInjector *IMS_PropulsionManager::getInjector(vector<int> propellant_ids, vector<float> ratio, double efficiency)
{
	//check if an injector with the specified mixture already exists
	for (UINT i = 0; i < injectors.size(); ++i)
	{
		if (injectors[i]->CompareMixture(propellant_ids, ratio, efficiency))
		{
			//an injector for this mixture already exists, do nothing
			return injectors[i];
		}
	}

	//no injector for this mixture exists yet, create one
	IMS_PropellantInjector *newinjector = new IMS_PropellantInjector(
		propellant_ids, ratio, efficiency, vessel);
	injectors.push_back(newinjector);

	//since this injector has just been created, the compatible tanks aren't connected to it yet
	for (map<int, vector<IMS_Storable*>>::iterator i = tanks.begin(); i != tanks.end(); ++i)
	{
		for (UINT j = 0; j < i->second.size(); ++j)
		{
			//only connect open tanks
			if (i->second[j]->IsAvailable())
			{
				newinjector->ConnectTank(i->second[j]);
			}
		}
	}
	return newinjector;
}


THRUSTER_HANDLE IMS_PropulsionManager::AddThruster(THRUSTERMODE *thrustermode, VECTOR3 &pos, VECTOR3 &dir)
{
	vector<int> &thpropellants = thrustermode->Propellants;
	vector<float> &thratio = thrustermode->Ratio;
	double thefficiency = thrustermode->Efficiency;

	//calculate position relative to CoG
	VECTOR3 finalpos = pos + vessel->GetCoGmanager()->GetCoG();
	THRUSTER_HANDLE newthruster = vessel->CreateThruster(pos, dir, thrustermode->Thrust, NULL, thrustermode->Isp);

	double maxmassflow = thrustermode->Thrust / thrustermode->Isp;
	//get a compatible injector, or create one if there isn't
	IMS_PropellantInjector *injector = getInjector(thpropellants, thratio, thefficiency);
	injector->ConnectThruster(newthruster, maxmassflow);

	//add exhausts to the thruster
	return newthruster;
}



void IMS_PropulsionManager::RemoveThruster(THRUSTER_HANDLE thruster, double maxflowrate, bool deletefromvessel)
{

	//walk through all injectors until the one is found that contains this thruster
	for (UINT i = 0; i < injectors.size(); ++i)
	{

		if (injectors[i]->DisconnectThruster(thruster, maxflowrate))
		{
			//check if the injector still has any thrusters connected. If not, it is no longer needed
			if (injectors[i]->GetNumberOfThrusters())
			{
				delete injectors[i];
				injectors.erase(injectors.begin() + i);
			}
			if (deletefromvessel)
			{
				vessel->DelThruster(thruster);
			}
			return;
		}
	}
}

void IMS_PropulsionManager::SwitchThrusterMode(THRUSTER_HANDLE thruster, THRUSTERMODE *mode)
{
	//calculate the flowrate so the injector can be updated accordingly, and remove the thruster from its injector
	double maxmassflow = mode->Thrust / mode->Isp;
	RemoveThruster(thruster, maxmassflow, false);

	//get the propper injector for the new mode, or create it if necessary
	vector<int> &thpropellants = mode->Propellants;
	vector<float> &thratio = mode->Ratio;
	double thefficiency = mode->Efficiency;
	
	//set new properties for the thruster
	vessel->SetThrusterIsp(thruster, mode->Isp);
	vessel->SetThrusterMax0(thruster, mode->Thrust);

	IMS_PropellantInjector *injector = getInjector(thpropellants, thratio, thefficiency);
	injector->ConnectThruster(thruster, maxmassflow);
}



void IMS_PropulsionManager::SetThrusterEnabled(THRUSTER_HANDLE thruster, bool enabled)
{
	for (UINT i = 0; i < injectors.size(); ++i)
	{
		if (enabled)
		{
			if (injectors[i]->EnableThruster(thruster))
			{
				break;
			}
		}
		else
		{
			if (injectors[i]->DisableThruster(thruster))
			{
				break;
			}
		}
	}
}


void IMS_PropulsionManager::AddThrusterToGroup(THRUSTER_HANDLE thruster, THGROUP_TYPE group)
{
	map<THGROUP_TYPE, vector<THRUSTER_HANDLE>>::iterator groupit = thgroups.find(group);

	if (groupit == thgroups.end())
	{
		//this thruster group has not yet had a thruster before, create one
		vector<THRUSTER_HANDLE> newgrouplist;
		newgrouplist.push_back(thruster);
		thgroups.insert(make_pair(group, newgrouplist));
	}
	else
	{
		//there's already thrusters in this list, let's just make sure that our thruster isn't already in there
		Olog::assertThat([groupit, thruster]() { return find(groupit->second.begin(), groupit->second.end(), thruster) == groupit->second.end(); }, "Thruster is already in group!");
		//now add the thruster to the list
		groupit->second.push_back(thruster);
	}

	//now update the thruster group in Orbiter
	updateThrusterGroup(group);
}


void IMS_PropulsionManager::RemoveThrusterFromGroup(THRUSTER_HANDLE thruster, THGROUP_TYPE group)
{
	//look for the thruster in the designated group
	map<THGROUP_TYPE, vector<THRUSTER_HANDLE>>::iterator groupit = thgroups.find(group);
	Olog::assertThat([groupit, this]() { return groupit != thgroups.end(); }, "Trying to remove a thruster from a group that was never created!");
	vector<THRUSTER_HANDLE>::iterator listit = find(groupit->second.begin(), groupit->second.end(), thruster);
	Olog::assertThat([listit, groupit]() { return listit != groupit->second.end(); }, "Trying to remove a thruster from a group it was never added to!");

	//remove the thruster from the list, and update the group
	groupit->second.erase(listit);
	updateThrusterGroup(group);
}


void IMS_PropulsionManager::updateThrusterGroup(THGROUP_TYPE group)
{
	//delete the thruster group. This call is NOT illegal when the group does not actually exist on the vessel.
	vessel->DelThrusterGroup(group);
	//find our thruster list for this group and make sure it actually exist in the manager
	map<THGROUP_TYPE, vector<THRUSTER_HANDLE>>::iterator groupit = thgroups.find(group);
	Olog::assertThat([groupit, this]() { return groupit != thgroups.end(); }, "Trying to update a thruster group without thrusters!");

	//recreate the thruster group, if it actually has any thrusters
	if (groupit->second.size() > 0)
	{
		//create the new group in the orbiter vessel.
		//maps guarantee memory persistance of their elements throughout their lifecycle, and
		//vectors guarantee the same as long as they aren't changed, so if this method is called
		//strictly after every modification to a group, the following is perfectly safe.
		vessel->CreateThrusterGroup(&groupit->second[0], groupit->second.size(), group);
	}
}



void IMS_PropulsionManager::PreStep(double simdt)
{
	bool injectormasschanged = false;
	for (UINT i = 0; i < injectors.size(); ++i)
	{
		if (injectors[i]->PreStep(simdt))
		{
			injectormasschanged = true;
		}
	}

	if (injectormasschanged)
	{
		addEvent(new MassHasChangedEvent(0));
	}

	IMS_Manager_Base::PreStep(simdt);
}


void IMS_PropulsionManager::AddExhausts(THRUSTER_HANDLE thruster, vector<THRUSTEREXHAUST> &exhausts)
{
	//create exhausts for the thruster and map their ids to the thruster
	for (UINT i = 0; i < exhausts.size(); ++i)
	{
		liveexhausts[thruster].push_back(vessel->AddExhaust(thruster,
			exhausts[i].length,
			exhausts[i].width,
			exhausts[i].pos, exhausts[i].dir));
	}
}

void IMS_PropulsionManager::RemoveExhausts(THRUSTER_HANDLE thruster)
{
	Olog::assertThat([this, thruster]() { return liveexhausts.find(thruster) != liveexhausts.end(); }, "Trying to remove exhausts that weren't added!");
	
	vector<UINT> &exhaustids = liveexhausts[thruster];
	for (UINT i = 0; i < exhaustids.size(); ++i)
	{
		//remove every exhaust from the vessel
		bool success = vessel->DelExhaust(exhaustids[i]);
		if (!success)
		{
			//if this shows up in the log, something is weird!
			Olog::debug("Orbiter could not remove exhaust!");
		}
	}
	//clear the exhausts from the definition list
	exhaustids.clear();
}


bool IMS_PropulsionManager::ProcessEvent(Event_Base *e)
{
	if (*e == CONSUMABLEADDEDEVENT)
	{
		//if the mass was previously zero, we must check if it's a tank that needs to be readded to an injector
		if (((ConsumableAddedEvent*)e)->GetPreviousMass() == 0.0)
		{
			IMS_Storable *storable = (IMS_Storable*)e->GetCaller();
			//if the storable isn't available, we don't have to add it anyways
			if (storable->IsAvailable())
			{
				//check if we have the thing registered as a tank
				map<int, vector<IMS_Storable*>>::iterator typelist = tanks.find(storable->GetConsumableId());
				if (typelist == tanks.end()) return false;

				vector<IMS_Storable*>::iterator it = find(typelist->second.begin(), typelist->second.end(), storable);
				if (it != typelist->second.end())
				{
					//it is indeed a tank, and the injectors have to know that it has some gojuice again!
					for (UINT i = 0; i < injectors.size(); ++i)
					{
						injectors[i]->ConnectTank(storable);
					}
					//since the caller was a propellant tank, I wouldn't know who else would be interested in the event.
					return true;
				}
			}
		}
	}
	else if (*e == SIMULATIONSTARTEDEVENT)
	{
		for (UINT i = 0; i < injectors.size(); ++i)
		{
			injectors[i]->FillInjectorDummy();
		}
	}
	return false;
}



void IMS_PropulsionManager::addTankToInjectors(IMS_Storable *tank)
{
	//if the tank is not available, it must not be added to an injector!
	if (tank->IsAvailable())
	{
		//the injectors will decide themselves whether they have use of the tank or not
		for (UINT i = 0; i < injectors.size(); ++i)
		{
			injectors[i]->ConnectTank(tank);
		}
	}
}


void IMS_PropulsionManager::removeTankFromInjectors(IMS_Storable *tank)
{
	//the injectors will see for themselves if the tank was acutally connected
	for (UINT i = 0; i < injectors.size(); ++i)
	{
		injectors[i]->DisconnectTank(tank);
	}
}






