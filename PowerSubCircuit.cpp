#include "Common.h"
#include <queue>
#include <set>
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"
#include "PowerCircuit_Base.h"
#include "PowerSubCircuit.h"

PowerSubCircuit::PowerSubCircuit(PowerParent *start, PowerBus *initiatingbus)
	: PowerCircuit_Base(start->GetCurrentOutputVoltage())
{
	buildCircuit(start, initiatingbus);
}


PowerSubCircuit::~PowerSubCircuit()
{
	for (auto i = powersources.begin(); i != powersources.end(); ++i)
	{
		(*i)->UnregisterContainingSubCircuit(this);
	}

	for (auto i = powerbuses.begin(); i != powerbuses.end(); ++i)
	{
		(*i)->UnregisterContainingSubCircuit(this);
	}
}


void PowerSubCircuit::AddPowerParent(PowerParent* parent)
{
	parent->RegisterContainingSubCircuit(this);
	PowerCircuit_Base::AddPowerParent(parent);
}


void PowerSubCircuit::buildCircuit(PowerParent *start, PowerBus *initiatingbus)
{
	queue<PowerParent*> parentstoprocess;
	set<PowerParent*> processedparents;			//keeps track of the elements already processed, so they aren't added twice.
	parentstoprocess.push(start);
	processedparents.insert(initiatingbus);
	processedparents.insert(start);


	while (parentstoprocess.size() > 0)
	{
		//take the next parent from the queue, add it to the subcircuit, then add all its parents to the queue.
		PowerParent *currentparent = parentstoprocess.front();
		parentstoprocess.pop();
		AddPowerParent(currentparent);

		if (currentparent->GetParentType() == PPT_BUS)
		{
			vector<PowerParent*> moreparents;
			((PowerBus*)(currentparent))->GetParents(moreparents);
			for (auto i = moreparents.begin(); i != moreparents.end(); ++i)
			{
				//check if we already processed this parent. 
				//This is necessary since bus-relationships are reciprocal (both parents and children of each other).
				pair<set<PowerParent*>::iterator, bool> parent_was_processed = processedparents.insert((*i));
				if (parent_was_processed.second)
				{
					parentstoprocess.push((*i));
				}
			}
		}
	}
}


double PowerSubCircuit::GetCurrentSurplus()
{
	return currentsurplus;
}


void PowerSubCircuit::Evaluate()
{
	if (statechange)
	{
		currentsurplus = 0;
		for (auto i = powersources.begin(); i != powersources.end(); ++i)
		{
			currentsurplus += (*i)->GetOutputCurrent();
		}

		for (auto i = powerbuses.begin(); i != powerbuses.end(); ++i)
		{
			double busresistance = (*i)->GetEquivalentResistance();
			//just because there are no consumers running on one bus doesn't mean there's a short-circuit. That would only be the case if nothing at all is running in the whole circuit.
			//dividing by zero would non the less provide for a little code short-circuit of our own...
			if (busresistance > 0)
			{
				currentsurplus -= voltage / busresistance;
			}
		}
		currentsurplus = max(0, currentsurplus);
		statechange = false;
	}
}