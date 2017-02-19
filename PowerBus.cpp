#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerBus.h"
#include "PowerConsumer.h"
#include "PowerCircuit.h"
#include "PowerCircuitManager.h"



PowerBus::PowerBus(double voltage, double maxamps, PowerCircuitManager *circuitmanager, UINT location_id)
	: PowerChild(POWERCHILD_TYPE::PCT_BUS, voltage, voltage), PowerParent(POWERPARENT_TYPE::PPT_BUS, voltage, voltage), maxcurrent(maxamps), locationid(location_id), circuitmanager(circuitmanager)
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


void PowerBus::Evaluate()
{
	//check if the state of any children of this object has changed at all.
	if (child_state_changed)
	{
		child_state_changed = false;
		double new_eq_resistance = 0;
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			//for the purpose of calculating the equivalent resistance of this bus, connected buses are ignored.
			//The circuit will calculate its total equivalent resistance by the equivalent resistance of every individual bus.
			if ((*i)->GetChildType() != PCT_BUS && (*i)->IsChildSwitchedIn())
			{
				new_eq_resistance += 1 / (*i)->GetChildResistance();
			}
		}
		equivalent_resistance = 1 / new_eq_resistance;
		circuit->RegisterStateChange();
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
	//this bus cannot reduce current by enough, save some checks and shut everything down!
	for (UINT i = children.size(); i > 0; --i)
	{
		if (children[i - 1]->GetChildType() == PCT_CONSUMER &&
			children[i - 1]->IsChildSwitchedIn())
		{
			PowerConsumer* consumer = (PowerConsumer*)children[i - 1];
			missing_current -= consumer->GetInputCurrent();
			consumer->SetRunning(false);
		}
		if (missing_current <= 0)
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
