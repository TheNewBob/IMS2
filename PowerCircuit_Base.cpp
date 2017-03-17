#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"
#include "PowerCircuit_Base.h"


PowerCircuit_Base::PowerCircuit_Base(double voltage)
	:voltage(voltage)
{
}


PowerCircuit_Base::~PowerCircuit_Base()
{
}


void PowerCircuit_Base::AddPowerParent(PowerParent *parent)
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

void PowerCircuit_Base::AddPowerSource(PowerSource *source)
{
	powersources.push_back(source);
}

void PowerCircuit_Base::AddPowerBus(PowerBus *bus)
{
	powerbuses.push_back(bus);
}


void PowerCircuit_Base::RemovePowerParent(PowerParent *parent)
{
	POWERPARENT_TYPE ptype = parent->GetParentType();
	if (ptype == PPT_BUS)
	{
		RemovePowerBus((PowerBus*)parent);
	}
	else if (ptype = PPT_SOURCE)
	{
		RemovePowerSource((PowerSource*)parent);
	}
}


void PowerCircuit_Base::RemovePowerSource(PowerSource *source)
{
	auto removesource = find(powersources.begin(), powersources.end(), source);
	assert(removesource != powersources.end() && "Attempting to remove PowerSOurce from Circuit that is not a member!");
	powersources.erase(removesource);
}

void PowerCircuit_Base::RemovePowerBus(PowerBus *bus)
{
	auto removebus = find(powerbuses.begin(), powerbuses.end(), bus);
	assert(removebus != powerbuses.end() && "Attempting to remove PowerSOurce from Circuit that is not a member!");
	powerbuses.erase(removebus);
}


void PowerCircuit_Base::GetPowerSources(vector<PowerSource*> &OUT_sources)
{
	OUT_sources = powersources;
}

void PowerCircuit_Base::GetPowerBuses(vector<PowerBus*> &OUT_buses)
{
	OUT_buses = powerbuses;
}

void PowerCircuit_Base::RegisterStateChange()
{
	statechange = true;
}

UINT PowerCircuit_Base::GetSize()
{
	return powersources.size() + powerbuses.size();
}

double PowerCircuit_Base::GetVoltage()
{
	return voltage;
}