#include "Common.h"
#include "PowerTypes.h"
#include "PowerParent.h"
#include "PowerChild.h"
#include "PowerCircuit_Base.h"
#include "PowerCircuit.h"
#include "PowerSubCircuit.h"

PowerParent::PowerParent(POWERPARENT_TYPE type, double minvoltage, double maxvoltage, bool switchable)
	: parenttype(type), parentcanswitch(switchable)

{ 
	outputvoltage.minimum = minvoltage;
	outputvoltage.maximum = maxvoltage;
};

PowerParent::~PowerParent()
{

}


bool PowerParent::CanConnectToChild(PowerChild *child, bool bidirectional)
{
	//the base conditions are that the child is not yet connected, 
	//and that the voltages of parent and child are compatible.
	if (find(children.begin(), children.end(), child) == children.end() &&
		outputvoltage.IsRangeCompatibleWith(child->GetInputVoltageInfo()))
	{
		if (bidirectional)
		{
			return child->CanConnectToParent(this, false);
		}
		return true;
	}
	return false;
}

void PowerParent::GetChildren(vector<PowerChild*> &OUT_children)
{
	OUT_children = children;
}

VOLTAGE_INFO PowerParent::GetOutputVoltageInfo()
{
	return outputvoltage;
}

double PowerParent::GetCurrentOutputVoltage()
{
	return outputvoltage.current;
}


bool PowerParent::IsParentSwitchedIn() 
{ 
	return parentswitchedin; 
}

bool PowerParent::IsParentSwitchable()
{
	return parentcanswitch;
}

void PowerParent::SetParentSwitchedIn(bool switchedin) 
{ 
	if (parentcanswitch && switchedin != parentswitchedin)
	{
		parentswitchedin = switchedin;
		circuit->RegisterStateChange();
	}
	
}

bool PowerParent::IsAutoswitchEnabled() 
{ 
	return parentautoswitch; 
}

void PowerParent::SetAutoswitchEnabled(bool enabled) 
{ 
	if (enabled != parentautoswitch)
	{
		parentautoswitch = enabled;
		RegisterChildStateChange();
	}
}


void PowerParent::RegisterChildStateChange() 
{ 
	child_state_changed = true;
	circuit->RegisterStateChange();
	for (auto i = containing_subcircuits.begin(); i != containing_subcircuits.end(); ++i)
	{
		(*i)->RegisterStateChange();
	}
}

POWERPARENT_TYPE PowerParent::GetParentType()
{
	return parenttype;
}

void PowerParent::SetCircuitToNull()
{
	circuit = NULL;
}

void PowerParent::SetCircuit(PowerCircuit *circuit)
{
	assert(circuit != NULL && "NULL-circuit passed to SetCircuit()! Use SetCircuitToNull() instead!");
	this->circuit = circuit;
}

PowerCircuit *PowerParent::GetCircuit()
{
	return circuit;
}

void PowerParent::RegisterContainingSubCircuit(PowerSubCircuit *subcircuit)
{
	assert(find(containing_subcircuits.begin(), containing_subcircuits.end(), subcircuit) == containing_subcircuits.end()
		&& "Attempting to register already registered subcircuit!");
	containing_subcircuits.push_back(subcircuit);
}

void PowerParent::UnregisterContainingSubCircuit(PowerSubCircuit *subcircuit)
{
	auto unregistercircuit = find(containing_subcircuits.begin(), containing_subcircuits.end(), subcircuit);
	assert(unregistercircuit != containing_subcircuits.end() && "Attempting to unregister subcircuit that was not registered!");
	containing_subcircuits.erase(unregistercircuit);
}


void PowerParent::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	//	assert(CanConnectToChild(child) && "PowerChild cannot be connected to PowerParent! Perform check if connection possible before calling this method!");

	children.push_back(child);

	if (bidirectional)
	{
		child->ConnectChildToParent(this, false);
	}
}

void PowerParent::DisconnectParentFromChild(PowerChild *child, bool bidirectional)
{
	auto childit = find(children.begin(), children.end(), child);
	assert(childit != children.end() && "PowerChild to be disconnected is not connected!");

	children.erase(childit);

	if (bidirectional)
	{
		child->DisconnectChildFromParent(this, false);
	}
}