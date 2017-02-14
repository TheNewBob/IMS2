#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerParent.h"
#include "PowerBus.h"
#include "PowerSource.h"
#include "PowerCircuit.h"

PowerSource::PowerSource(double minvoltage, double maxvoltage, double maxpower, double internalresistance) 
	: PowerParent(POWERPARENT_TYPE::PPT_SOURCE, minvoltage, maxvoltage), baseinternalresistance(internalresistance)
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

double PowerSource::GetBaseInternalResistance()
{
	return baseinternalresistance;
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
			circuit->RegisterStateChange();
		}
	}
}


void PowerSource::Evaluate()
{
	//powersources are controlled by the circuit, so all we have to do is to notify the circuit that things have changed.
	if (child_state_changed)
	{
		circuit->RegisterStateChange();
	}
}

bool PowerSource::CanConnectToChild(PowerChild *child)
{
	//check if no children are connected yet, the child is a bus, and the voltage is compatible.
	if (children.size() < 1 &&
		child->GetChildType() == POWERCHILD_TYPE::PCT_BUS &&
		child->CanConnectToParent(this))
	{
		if (outputvoltage.IsRangeCompatibleWith(child->GetInputVoltageInfo()))
		{
			return true;
		}
	}
	return false;
}


PowerCircuit *PowerSource::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	//establish the connection
	PowerCircuit *result = PowerParent::ConnectParentToChild(child, bidirectional);

	//comply to the childs input power. Tests for compatibility were already done at this point.
	outputvoltage.current = child->GetCurrentInputVoltage();
	circuit->RegisterStateChange();
	return result;
}