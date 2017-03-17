#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"
#include "PowerCircuit_Base.h"
#include "PowerCircuit.h"
#include "PowerCircuitManager.h"
#include <queue>
#include <set>


PowerCircuitManager::PowerCircuitManager()
{
}


PowerCircuitManager::~PowerCircuitManager()
{
}


PowerCircuit *PowerCircuitManager::CreateCircuit(PowerBus *initialbus)
{
	assert(initialbus->GetCircuit() == NULL && "New PowerCircuit can only be created with a bus that is not member of another circuit!");

	PowerCircuit *newcircuit = new PowerCircuit(initialbus);
	circuits.push_back(newcircuit);
	return newcircuit;
}

void PowerCircuitManager::DeletePowerCircuit(PowerCircuit *circuit)
{
	auto i = find(circuits.begin(), circuits.end(), circuit);
	assert( i != circuits.end() && "Cannot delete PowerCircuit that is not known to this PowerCircuitManager!");

	circuits.erase(i);
	delete circuit;
}

void PowerCircuitManager::MergeCircuits(PowerCircuit *circuit_a, PowerCircuit *circuit_b)
{
	assert(find(circuits.begin(), circuits.end(), circuit_a) != circuits.end() && "Attempting to merge circuit that is not managed by this PowerCIrcuitManager!");
	assert(find(circuits.begin(), circuits.end(), circuit_b) != circuits.end() && "Attempting to merge circuit that is not managed by this PowerCIrcuitManager!");

	vector<PowerSource*> sources;
	circuit_b->GetPowerSources(sources);
	for (auto source = sources.begin(); source != sources.end(); ++source)
	{
		circuit_a->AddPowerSource((*source));
	}

	vector<PowerBus*> buses;
	circuit_b->GetPowerBuses(buses);
	for (auto bus = buses.begin(); bus != buses.end(); ++bus)
	{
		circuit_a->AddPowerBus((*bus));
	}

	DeletePowerCircuit(circuit_b);
}


void PowerCircuitManager::SplitCircuit(PowerCircuit *circuit, PowerBus *split_at, PowerParent *split_from)
{
	//set to store already processed parent to avoid endless recursion between buses.
	set<PowerParent*> processed_parents;
	processed_parents.insert(split_from);
	processed_parents.insert(split_at);

	//queue to walk through all descendants of split_at breadth first.
	queue<PowerParent*> parents_to_process;
	parents_to_process.push(split_at);
	PowerCircuit *newcircuit = NULL;

	while (parents_to_process.size() > 0)
	{
		//take the next item from the queue
		PowerParent *currentparent = parents_to_process.front();
		parents_to_process.pop();

		//remove the child from its old circuit and add it to the new one.
		//create the circuit if it doesn't exist yet.
		currentparent->GetCircuit()->RemovePowerParent(currentparent);
		if (newcircuit == NULL)
		{
			//not as unsave as it looks. If the circuit does not yet exist, the current parent must necessarily be split_at.
			newcircuit = CreateCircuit((PowerBus*)currentparent);
		}
		else
		{
			newcircuit->AddPowerParent(currentparent);
		}

		if (currentparent->GetParentType() == PPT_BUS)
		{
			//walk through the buses and add their *parents* to the queue. We don't actually care about children.
			PowerBus *currentbus = (PowerBus*)currentparent;
			for (auto i = currentbus->parents.begin(); i != currentbus->parents.end(); ++i)
			{
				//check if the parent was already processed, if not, add it to the queue.
				pair<set<PowerParent*>::iterator, bool> parent_was_processed = processed_parents.insert((*i));
				if (parent_was_processed.second)
				{
					parents_to_process.push((*i));
				}
			}
		}
	}

	if (circuit->powerbuses.size() == 0)
	{
		DeletePowerCircuit(circuit);
	}
}


void PowerCircuitManager::Evaluate()
{
	for (auto circuit = circuits.begin(); circuit != circuits.end(); ++circuit)
	{
		(*circuit)->Evaluate();
	}
}

void PowerCircuitManager::GetPowerCircuits(vector<PowerCircuit*> &OUT_circuits)
{
	OUT_circuits = circuits;
}


UINT PowerCircuitManager::GetSize()
{
	return circuits.size();
}