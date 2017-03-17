#include "Common.h"
#include "PowerTypes.h"
#include "PowerParent.h"
#include "PowerChild.h"

PowerChild::PowerChild(POWERCHILD_TYPE type, double minvoltage, double maxvoltage, bool switchable)
	: childtype(type), childcanswitch(switchable)
{
	inputvoltage.minimum = minvoltage;
	inputvoltage.maximum = maxvoltage;
}


PowerChild::~PowerChild()
{

}

void PowerChild::GetParents(vector<PowerParent*> &OUT_parents)
{
	OUT_parents = parents;
}

bool PowerChild::IsChildSwitchedIn() 
{ 
	return childswitchedin; 
}

bool PowerChild::IsChildSwitchable()
{
	return childcanswitch;
}

void PowerChild::SetChildSwitchedIn(bool switchedin) 
{ 
	if (switchedin != childswitchedin)
	{
		childswitchedin = switchedin;
		registerStateChangeWithParents();
	}
}

VOLTAGE_INFO PowerChild::GetInputVoltageInfo()
{
	return inputvoltage;
}

double PowerChild::GetCurrentInputVoltage()
{
	return inputvoltage.current;
}


void PowerChild::registerStateChangeWithParents()
{
	for (auto i = parents.begin(); i != parents.end(); ++i)
	{
		(*i)->RegisterChildStateChange();
	}
}


POWERCHILD_TYPE PowerChild::GetChildType() 
{ 
	return childtype; 
}


bool PowerChild::CanConnectToParent(PowerParent *parent, bool bidirectional)
{
	if (bidirectional)
	{
		return parent->CanConnectToChild(this, false);
	}
	return true;
}


void PowerChild::connectChildToParent(PowerChild *child, PowerParent *parent, bool bidirectional)
{
	assert(find(child->parents.begin(), child->parents.end(), parent) == child->parents.end() && "PowerChild is already registered!");

	child->parents.push_back(parent);

	if (bidirectional)
	{
		parent->ConnectParentToChild(child, false);
	}
}

void PowerChild::disconnectChildFromParent(PowerChild *child, PowerParent *parent, bool bidirectional)
{
	auto parentit = find(child->parents.begin(), child->parents.end(), parent);
	assert(parentit != child->parents.end() && "PowerChild is not registered!");

	child->parents.erase(parentit);

	if (bidirectional)
	{
		parent->DisconnectParentFromChild(child, false);
	}
}
