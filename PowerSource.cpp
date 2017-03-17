#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerBus.h"
#include "PowerSource.h"
#include "PowerCircuit_Base.h"
#include "PowerCircuit.h"

PowerSource::PowerSource(double minvoltage, double maxvoltage, double maxpower, double internalresistance, UINT location_id, bool global)
	: PowerParent(POWERPARENT_TYPE::PPT_SOURCE, minvoltage, maxvoltage), internalresistance(internalresistance), maxpowerout(maxpower), locationid(location_id), global(global)
{
	
}

PowerSource::~PowerSource()
{

}


double PowerSource::GetMaxPowerOutput()
{
	return maxpowerout;
}

double PowerSource::GetCurrentPowerOutput()
{
	return outputvoltage.current * curroutputcurrent;
}


double PowerSource::GetInternalResistance()
{
	return internalresistance;
}

double PowerSource::GetOutputCurrent()
{
	return curroutputcurrent;
}


double PowerSource::GetMaxOutputCurrent(bool force)
{
	if (parentswitchedin ||
		(force && parentautoswitch))
	{
		//Communicate maximum power output if switched into the circuit,
		//or if reply is forced and autoswitch is enabled.
		return maxoutcurrent;
	}
	else
	{
		//if switched out and not authorised to switch back in when needed, source can't deliver any current!
		return 0;
	}
}

void PowerSource::SetRequestedCurrent(double amps)
{
	//if more amps are requested than can be delivered, the circuit screwed up badly!
	assert(amps <= maxoutcurrent && "Requested more current than can be delivered!");
	if (amps != curroutputcurrent)
	{
		curroutputcurrent = amps;
		RegisterChildStateChange();
	}
}

void PowerSource::SetMaxPowerOutput(double watts)
{
	if (watts != maxpowerout)
	{
		maxpowerout = watts;
		maxoutcurrent = maxpowerout / outputvoltage.current;
		if (curroutputcurrent > maxoutcurrent)
		{
			curroutputcurrent = maxoutcurrent;
			RegisterChildStateChange();
		}
	}
}


void PowerSource::Evaluate()
{
	assert(true && "Evaluate on PowerSource should never be called!");
}

bool PowerSource::CanConnectToChild(PowerChild *child, bool bidirectional)
{
	//check if no children are connected yet, the child is a bus, and the voltage is compatible.
	if (children.size() < 1 &&
		child->GetChildType() == POWERCHILD_TYPE::PCT_BUS &&
		PowerParent::CanConnectToChild(child, bidirectional))
	{
		return true;
	}
	return false;
}


void PowerSource::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	//establish the connection
	PowerParent::ConnectParentToChild(child, bidirectional);

	//comply to the childs input power. Tests for compatibility were already done at this point.
	outputvoltage.current = child->GetCurrentInputVoltage();
	maxoutcurrent = maxpowerout / outputvoltage.current;
	RegisterChildStateChange();
}

void PowerSource::SetCircuitToNull()
{
	//the source is removed from a circuit, shut it down!
	circuit = NULL;
	curroutputcurrent = 0;
}


UINT PowerSource::GetLocationId()
{
	return locationid;
}

bool PowerSource::IsGlobal()
{
	return global;
}

