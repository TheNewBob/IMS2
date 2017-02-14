#include "Common.h"
#include "PowerTypes.h"
#include "PowerParent.h"
#include "PowerChild.h"
#include "PowerCircuit.h"

PowerParent::PowerParent(POWERPARENT_TYPE type, double minvoltage, double maxvoltage)
	: parenttype(type) 

{ 
	outputvoltage.minimum = minvoltage;
	outputvoltage.maximum = maxvoltage;
};

PowerParent::~PowerParent()
{

}


PowerCircuit *PowerParent::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	assert(CanConnectToChild(child) && "PowerChild cannot be connected to PowerParent! Perform check if connection possible before calling this method!");

	children.push_back(child);

	if (bidirectional)
	{
		return child->ConnectChildToParent(this, false);
	}

	return NULL;
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

bool PowerParent::CanConnectToChild(PowerChild *child)
{
	//the base conditions are that the child is not yet connected, 
	//and that the voltages of parent and child are compatible.
	if (find(children.begin(), children.end(), child) == children.end() &&
		outputvoltage.IsRangeCompatibleWith(child->GetInputVoltageInfo()))
	{
		return true;
	}
	return false;
}

void PowerParent::GetChildren(vector<PowerChild*> OUT_children)
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

void PowerParent::SetParentSwitchedIn(bool switchedin) 
{ 
	if (switchedin != parentswitchedin)
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
		circuit->RegisterStateChange();
	}
}


void PowerParent::RegisterChildStateChange() 
{ 
	child_state_changed = true; 
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
	this->circuit = circuit;
}

PowerCircuit *PowerParent::GetCircuit()
{
	return circuit;
}