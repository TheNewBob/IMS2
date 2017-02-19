#include "Common.h"
#include "PowerTypes.h"
#include "PowerParent.h"
#include "PowerChild.h"

PowerChild::PowerChild(POWERCHILD_TYPE type, double minvoltage, double maxvoltage)
	: childtype(type)
{
	inputvoltage.minimum = minvoltage;
	inputvoltage.maximum = maxvoltage;
}


PowerChild::~PowerChild()
{

}


void PowerChild::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	assert(find(parents.begin(), parents.end(), parent) == parents.end() && "PowerChild is already registered!");

	parents.push_back(parent);

	if (bidirectional)
	{
		parent->ConnectParentToChild(this, false);
	}
}

void PowerChild::DisconnectChildFromParent(PowerParent *parent, bool bidirectional)
{
	auto parentit = find(parents.begin(), parents.end(), parent);
	assert(parentit != parents.end() && "PowerChild is not registered!");

	parents.erase(parentit);

	if (bidirectional)
	{
		parent->DisconnectParentFromChild(this, false);
	}
}

void PowerChild::GetParents(vector<PowerParent*> &OUT_parents)
{
	OUT_parents = parents;
}

bool PowerChild::IsChildSwitchedIn() 
{ 
	return childswitchedin; 
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
