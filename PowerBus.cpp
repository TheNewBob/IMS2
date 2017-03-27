#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerBus.h"
#include "PowerConsumer.h"
#include "PowerCircuit_Base.h"
#include "PowerCircuit.h"
#include "PowerSubCircuit.h"
#include "PowerCircuitManager.h"



PowerBus::PowerBus(double voltage, double maxamps, PowerCircuitManager *circuitmanager, UINT location_id)
	: PowerChild(POWERCHILD_TYPE::PCT_BUS, voltage, voltage, false), PowerParent(POWERPARENT_TYPE::PPT_BUS, voltage, voltage, false), maxcurrent(maxamps), locationid(location_id), circuitmanager(circuitmanager)
{
	outputvoltage.current = voltage;
	inputvoltage.current = voltage;
}


PowerBus::~PowerBus()
{

}


double PowerBus::GetEquivalentResistance()
{
	return equivalent_resistance;
}


double PowerBus::GetCurrent()
{
	return throughcurrent;
}


double PowerBus::GetMaxCurrent()
{
	return maxcurrent;
}


void PowerBus::SetCurrent(double amps)
{
	//there's no check here to limit the current flow. The bus can be overloaded, and will be overloaded with bad design,
	//but that will result in heat and eventual breakdown, not in limiting.
	throughcurrent = amps;
}


void PowerBus::SetMaxCurrent(double amps)
{
	maxcurrent = amps;
}


void PowerBus::Evaluate(double deltatime)
{
	//check if the state of any children of this object has changed at all.
	if (child_state_changed)
	{
		double new_eq_resistance = 0;
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			//for the purpose of calculating the equivalent resistance of this bus, connected buses are ignored.
			//The circuit will calculate its total equivalent resistance by the equivalent resistance of every individual bus.
			if ((*i)->GetChildType() == PCT_CONSUMER && (*i)->IsChildSwitchedIn())
			{
				new_eq_resistance += 1 / (*i)->GetChildResistance();
				//we will assume that there is enough current available to run everything. If not, the consumers will be notified before this frames processing is over.
				((PowerConsumer*)(*i))->SetRunning(true);
			}
		}
		equivalent_resistance = 1 / new_eq_resistance;
		RegisterChildStateChange();
		child_state_changed = false;
	}
}

void PowerBus::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	PowerParent::ConnectParentToChild(child, bidirectional);

	if (!bidirectional && child->GetChildType() == PCT_BUS &&
		find(parents.begin(), parents.end(), (PowerBus*)(child)) == parents.end())
	{
		PowerBus *otherbus = (PowerBus*)(child);
		otherbus->ConnectParentToChild(this);
	}
}

void PowerBus::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	//when a bus gets connected as a child, it is its job to either integrate into the circuit of the parent,
	//or, if the parent isn't yet a member of a circuit, create one and integrate the parent into it.
	if (circuit == NULL)
	{
		//this isn't in a circuit yet, is the parent?
		PowerCircuit *newcircuit = parent->GetCircuit();
		if (newcircuit == NULL)
		{
			//parent isn't in a circuit either. Let's create one!
			newcircuit = circuitmanager->CreateCircuit(this);
			newcircuit->AddPowerParent(parent);
		}
		else
		{
			//parent is in a circuit, but this isn't. integrate into parent circuit.
			newcircuit->AddPowerBus(this);
		}
		circuit = newcircuit;
	}
	else if (circuit != parent->GetCircuit())
	{
		//this is already part of a circuit, and it's not the same as the parents. But what if the parent is already part of another circuit?
		if (parent->GetCircuit() != NULL)
		{
			//integrate this circuit into the parents circuit
			circuitmanager->MergeCircuits(parent->GetCircuit(), circuit);
		}
		else
		{
			//the parent doesn't have a circuit yet, so integrate it into this one.
			circuit->AddPowerParent(parent);
		}
	}
	//connect the darn things already!
	PowerChild::ConnectChildToParent(parent, bidirectional);

	//a special thing about buses is that they have reciprocal relationships:
	//they are both parents of each other, and thus also are children of each other.
	//Their effective role depends solely on which side the current is coming from.
	if (bidirectional == false && parent->GetParentType() == PPT_BUS &&
		find(children.begin(), children.end(), (PowerBus*)(parent)) == children.end())
	{
		PowerBus *otherbus = (PowerBus*)(parent);
		otherbus->ConnectChildToParent(this);
	}
}


void PowerBus::DisconnectChildFromParent(PowerParent *parent, bool bidirectional)
{
	PowerChild::DisconnectChildFromParent(parent, bidirectional);

	if (bidirectional && parent->GetParentType() == PPT_BUS)
	{
		//bus relations are reciprocal, so the parent is also our child, and that relation needs to be severed too.
		DisconnectParentFromChild((PowerBus*)parent, true);
	}

	if (circuit == parent->GetCircuit())
	{
		//at this point, all relations are resolved, and we have to move this bus and everything connected to it to a new circuit.
		circuitmanager->SplitCircuit(circuit, this, parent);
	}
	
}

void PowerBus::DisconnectParentFromChild(PowerChild *child, bool bidirectional)
{
	PowerParent::DisconnectParentFromChild(child, bidirectional);
}


void PowerBus::RebuildFeedingSubcircuits()
{
	//destroy all subcircuits.
	for (auto i = feeding_subcircuits.begin(); i != feeding_subcircuits.end(); ++i)
	{
		delete (*i);
	}

	feeding_subcircuits.clear();

	//construct a subcircuit for every parent.
	for (auto i = parents.begin(); i != parents.end(); ++i)
	{
		feeding_subcircuits.push_back(new PowerSubCircuit((*i), this));
	}
}


bool PowerBus::CanConnectToChild(PowerChild *child, bool bidirectional)
{
	if (child != this &&
		PowerParent::CanConnectToChild(child, bidirectional))
	{
		return true;
	}
	return false;
}

bool PowerBus::CanConnectToParent(PowerParent *parent, bool bidirectional)
{
	//a Bus can always connect to a parent that gives its ok,
	//provided it is not trying to connect to an element in the same circuit.
	//this avoidance of circular connections makes the computations in the circuit a lot easier!
	if (parent->GetCircuit() != circuit || circuit == NULL)
	{
		return PowerChild::CanConnectToParent(parent, bidirectional);
	}
	return false;
}


double PowerBus::GetChildResistance()
{
	//the own resistance of a bus is always zero.
	return 0.0;
}


double PowerBus::ReduceCurrentFlow(double missing_current)
{
	//note that comparison operations in here are a bit unusual.
	//They are this way because I have to deal with double rounding precision,
	//and because I don't want to introduce an unnecessary dependency to do the same work.
	for (UINT i = children.size(); i > 0; --i)
	{
		if (children[i - 1]->GetChildType() == PCT_CONSUMER &&
			children[i - 1]->IsChildSwitchedIn())
		{
			PowerConsumer* consumer = (PowerConsumer*)children[i - 1];
			if (consumer->IsRunning())
			{
				double consumedcurrent = consumer->GetInputCurrent();
				if ((missing_current - consumedcurrent) > -1e-9)		//Read: >=, but only to 9 digits behind the point.
				{
					//this consumer won't get enough power.
					consumer->SetRunning(false);
					missing_current -= consumedcurrent;
				}
				else
				{
					bool couldsetcurrent = consumer->SetConsumerLoadForCurrent(consumedcurrent - missing_current);
					if (couldsetcurrent)
					{
						//the consumers load could be reduced to eat exactly the amount of current we still had available.
						missing_current = 0;
					}
					else
					{
						//the consumer didn't get enough current for operation and went into standby. 
						double still_missing_current = missing_current - (consumedcurrent - consumer->GetInputCurrent());
						
						if (still_missing_current < -1e-9)     //Read: < , with a possible rounding error beyond 9 digits behind the point counting as equal
						{
							//The fact that the consumer went into standby could mean that we have some current left over now.
							//this is indicated by negative missing current. In this case, leave things as they are.
							missing_current = still_missing_current;
						}
						else
						{
							//On the other hand, if there's still current missing, there isn't even enough for the consumer to remain in standby.
							consumer->SetRunning(false);
							missing_current -= consumedcurrent;
						}
					}
				}
			}
		}
		if (missing_current < 1e-9 && missing_current > -1e-9)
		{
			//rounding to exact zero on the 9th digit behind the period to prevent this process from building cumulative errors.
			missing_current = 0;
		}
		else if (missing_current <= 0)
		{
			return missing_current;
		}
	}
	return missing_current;
}


UINT PowerBus::GetLocationId()
{
	return locationid;
}

bool PowerBus::IsGlobal()
{
	return true;
}


void PowerBus::CalculateTotalCurrentFlow(double deltatime)
{

	//the current flowing through this bus is really just the current surplus of all feeding subcircuits.
	throughcurrent = 0;
	for (auto i = feeding_subcircuits.begin(); i != feeding_subcircuits.end(); ++i)
	{
		(*i)->Evaluate(deltatime);
		throughcurrent += (*i)->GetCurrentSurplus();
	}
}

