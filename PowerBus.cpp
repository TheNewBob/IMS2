#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerBus.h"
#include "PowerConsumer.h"
#include "PowerCircuit.h"



PowerBus::PowerBus(double voltage, double maxamps) 
	: PowerChild(POWERCHILD_TYPE::PCT_BUS, voltage, voltage), PowerParent(POWERPARENT_TYPE::PPT_BUS, voltage, voltage), maxcurrent(maxamps)
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


PowerCircuit *PowerBus::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	PowerCircuit *result = NULL;
	//when a bus gets connected as a child, it is its job to either integrate into the circuit of the parent,
	//or, if the parent isn't yet a member of a circuit, create one and integrate the parent into it.
	if (circuit == NULL)
	{
		//this isn't in a circuit yet, is the parent?
		PowerCircuit *newcircuit = parent->GetCircuit();
		if (newcircuit == NULL)
		{
			//parent isn't in a circuit either. Let's create one!
			newcircuit = new PowerCircuit(inputvoltage.current, this);
			parent->SetCircuit(newcircuit);
			newcircuit->AddPowerParent(parent);
			result = newcircuit;
		}
		else
		{
			//parent is in a circuit, but this isn't. integrate into parent circuit.
			circuit = newcircuit;
			circuit->AddPowerBus(this);
		}
	}
	else
	{
		//this is already part of a circuit, ergo the parent isn't.
		parent->SetCircuit(circuit);
	}
	//connect the darn things already!
	PowerChild::ConnectChildToParent(parent, bidirectional);
	return result;
}

bool PowerBus::CanConnectToChild(PowerChild *child)
{
	if (child->CanConnectToParent(this) &&
		PowerParent::CanConnectToChild(child))
	{
		return true;
	}
	return false;
}

bool PowerBus::CanConnectToParent(PowerParent *parent)
{
	//a Bus can always connect to a parent that gives its ok.
	return true;
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
