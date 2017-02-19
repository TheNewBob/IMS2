#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"
#include "PowerCircuit.h"
#include "PowerCircuitManager.h"


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