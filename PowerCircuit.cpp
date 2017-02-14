#include "Common.h"
#include "PowerTypes.h"
#include "PowerCircuit.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"


PowerCircuit::PowerCircuit(double voltage, PowerBus *initialbus) : voltage(voltage)
{
	AddPowerBus(initialbus);
}

PowerCircuit::~PowerCircuit()
{

}


void PowerCircuit::AddPowerParent(PowerParent *parent)
{
	POWERPARENT_TYPE ptype = parent->GetParentType();
	if (ptype == PPT_BUS)
	{
		AddPowerBus((PowerBus*)parent);
	}
	else if (ptype = PPT_SOURCE)
	{
		AddPowerSource((PowerSource*)parent);
	}
}

void PowerCircuit::AddPowerSource(PowerSource *source)
{
	assert(find(powersources.begin(), powersources.end(), source) == powersources.end() && "PowerSource was already added to circuit!");
	powersources.push_back(source);
}

void PowerCircuit::AddPowerBus(PowerBus *bus)
{
	assert(find(powerbuses.begin(), powerbuses.end(), bus) == powerbuses.end() && "PowerSource was already added to circuit!");
	powerbuses.push_back(bus);
}

void PowerCircuit::RemovePowerSource(PowerSource *source)
{
	auto removesource = find(powersources.begin(), powersources.end(), source);
	assert(removesource != powersources.end() && "Attempting to remove PowerSOurce from Circuit that is not a member!");
	(*removesource)->SetCircuitToNull();
	powersources.erase(removesource);
}

void PowerCircuit::RemovePowerBus(PowerBus *bus)
{
	auto removebus = find(powerbuses.begin(), powerbuses.end(), bus);
	assert(removebus != powerbuses.end() && "Attempting to remove PowerSOurce from Circuit that is not a member!");
	(*removebus)->SetCircuitToNull();
	powerbuses.erase(removebus);
}


void PowerCircuit::GetPowerSources(vector<PowerSource*> &OUT_sources)
{
	OUT_sources = powersources;
}

void PowerCircuit::GetPowerSources(vector<PowerBus*> &OUT_buses)
{
	OUT_buses = powerbuses;
}

void PowerCircuit::RegisterStateChange()
{
	statechange = true;
}

void PowerCircuit::Evaluate()
{
	if (statechange)
	{
		statechange = false;
		calculateEquivalentResistance();
		//calculate the current we actually need.
		total_circuit_current = voltage / equivalent_resistance;
	}



}


void PowerCircuit::calculateEquivalentResistance()
{

	double new_eq_resistance = 0;
	for (auto i = powerbuses.begin(); i != powerbuses.end(); ++i)
	{
		//the equivalent resistance of the circuit is built from the equivalent resistances of all the buses
		new_eq_resistance += 1 / (*i)->GetEquivalentResistance();
	}
	equivalent_resistance = 1 / new_eq_resistance;
}


void PowerCircuit::distributeCurrentDraw(bool force)
{

	//walk through the powersource and see which ones are providing power
	double total_available_current = 0;
	vector<POWERSOURCE_STATS*> involved_sources;        //will keep track of the powersources involved in feeding the circuit.
	for (UINT i = 0; i < powersources.size(); ++i)
	{
		if (powersources[i]->IsParentSwitchedIn())
		{
			//the powersource is switched in. Check if we'd already have enough current, and if yes, switch it out if autoswitch is enabled
			if (total_available_current >= total_circuit_current && powersources[i]->IsAutoswitchEnabled())
			{
				powersources[i]->SetParentSwitchedIn(false);
			}
			else
			{
				involved_sources.push_back(new POWERSOURCE_STATS(powersources[i], force));
				total_available_current += involved_sources.back()->maxcurrent;
			}
		}
	}

	if (total_available_current < total_circuit_current)
	{
		//We don't have enough power! Switch in sources that are on standby!
		double missing_current = switchInPowerSourcesOnStandby(involved_sources, total_circuit_current - total_available_current);
		if (missing_current > 0)
		{
			//we switched in all the sources we are allowed to, but we still don't have enough current! Some things will have to go!
			reduceCircuitCurrentBy(missing_current);

		}
	}
	//Now the circuit is stable and able to provide enough current for anything still running.
	//calculate how much every powersource will provide, and sort out all sources that are not limited by their maximum output
	calculateCurrentDraw(involved_sources, total_circuit_current, force);

	//apply changes to powersources and deallocate the stats.
	for (UINT i = 0; i < involved_sources.size(); ++i)
	{
		involved_sources[i]->Apply();
		delete involved_sources[i];
	}
}


double PowerCircuit::switchInPowerSourcesOnStandby(vector<POWERSOURCE_STATS*> &IN_OUT_involved_sources, double missing_current)
{
	for (UINT i = 0; i < powersources.size(); ++i)
	{
		//if we have enough current, exit.
		if (missing_current > 0)
		{
			if (powersources[i]->IsAutoswitchEnabled() &&
				!powersources[i]->IsParentSwitchedIn())
			{
				//the powersource is on standby, switch it in and see how much current it provides.
				powersources[i]->SetParentSwitchedIn(true);
				IN_OUT_involved_sources.push_back(new POWERSOURCE_STATS(powersources[i], true));
				missing_current -= IN_OUT_involved_sources.back()->maxcurrent;
			}
		}
		else
		{
			break;
		}
	}
	return max(missing_current, 0);
}


void PowerCircuit::reduceCircuitCurrentBy(double missing_current)
{
	for (UINT i = powerbuses.size(); i > 0; --i)
	{
		missing_current = powerbuses[i - 1]->ReduceCurrentFlow(missing_current);
		if (missing_current <= 0)
		{
			return;
		}
	}
	assert(missing_current < 0 && "There's still too little current although nothing is running. Something's obviously not behaving as intended!");
}


void PowerCircuit::calculateCurrentDraw(vector<POWERSOURCE_STATS*> non_limited_sources, double required_current, bool force)
{
	//get the sum of equivalent resistances
	double sum_eq_resistances = getSumOfEquivalentResistances(non_limited_sources);

	//calculate current drawn from each source and immediately get rid of those who hit limit
	bool circuit_stable = true;
	for (UINT i = non_limited_sources.size(); i > 0; --i)
	{
		//the equation: <Sum of currents> / (<sum of (1 / internal resistance)> / <(1 / internal resistance of power source)>) = current drawn from this particular power source.
		non_limited_sources[i - 1]->SetRequestedCurrent(required_current / (sum_eq_resistances / (1 / non_limited_sources[i - 1]->baseresistance)));
		if (non_limited_sources[i - 1]->limited)
		{
			double newrequiredcurrent = required_current - non_limited_sources[i - 1]->deliveredcurrent;
			non_limited_sources.erase(non_limited_sources.begin() + (i - 1));
			if (i - 1 < non_limited_sources.size())
			{
				//if a source is limited in the midst of the operation, it will affect the sources further up.
				//Unfortunately, there's nothing to it but to redo those operations again, which we'll do by recursing!
				calculateCurrentDraw(non_limited_sources, newrequiredcurrent, force);
				break;
			}
		}
	}
}



double PowerCircuit::getSumOfEquivalentResistances(vector<POWERSOURCE_STATS*> &involved_sources)
{
	double sum_eq_resistances = 0;
	for (auto i = involved_sources.begin(); i != involved_sources.end(); ++i)
	{
		sum_eq_resistances += 1 / (*i)->baseresistance;
	}
	return sum_eq_resistances;
}


