#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerConsumer.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerBus.h"
#include "PowerCircuit_Base.h"
#include "PowerCircuit.h"
#include "PowerConverter.h"


PowerConverter::PowerConverter(double minvoltage,
							   double maxvoltage,
							   double maxpower,
							   double conversionefficiency,
							   double internalresistance,
							   UINT location_id,
							   bool global)
	: PowerSource(minvoltage, maxvoltage, maxpower, internalresistance, location_id, global),
	  PowerConsumer(minvoltage, maxvoltage, maxpower, location_id, -1, 0, global),
	  conversionefficiency(conversionefficiency)
{

}

PowerConverter::~PowerConverter()
{

}

double PowerConverter::GetConversionEfficiency()
{
	return conversionefficiency;
}


void PowerConverter::SetConversionEfficiency(double efficiency)
{
	assert(efficiency > 0 && efficiency <= 1 && "Trying to set invalid conversion efficiency!");
	conversionefficiency = efficiency;
}



bool PowerConverter::CanConnectToChild(PowerChild *child, bool bidirectional)
{
	//first check if this could even connect to the child as a normal power source.
	if (PowerSource::CanConnectToChild(child, bidirectional))
	{
		//If already connected to a parent, make sure the childs voltage is lower.
		if (parents.size() > 0 && 
			((PowerBus*)child)->GetCurrentInputVoltage() < ((PowerBus*)parents[0])->GetCurrentOutputVoltage())
		{
			return true;
		}
	}
	return false;
}


bool PowerConverter::CanConnectToParent(PowerParent *parent, bool bidirectional)
{
	//first check if this could even connect to the parent as a normal consumer.
	if (PowerConsumer::CanConnectToParent(parent, bidirectional))
	{
		//If already connected to a child, make sure the parents voltage is higher.
		if (children.size() > 0 &&
			((PowerBus*)parent)->GetCurrentInputVoltage() > ((PowerBus*)children[0])->GetCurrentOutputVoltage())
		{
			return true;
		}
	}
	return false;
}


void PowerConverter::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	//connect like a normal consumer, but note the parents circuit
	PowerConsumer::ConnectChildToParent(parent, bidirectional);
	childcircuit = parent->GetCircuit();
}


void PowerConverter::Evaluate(double deltatime)
{
	//TODO?	
}

bool PowerConverter::SetConsumerLoad(double load)
{
	//we don't need to do anything different than a standard consumer, except let the OTHER circuit know that the state changed.
	RegisterChildStateChange();
	return PowerConsumer::SetConsumerLoad(load);
	
}


void PowerConverter::SetRequestedCurrent(double amps)
{
	//let the power source do its thing
	PowerSource::SetRequestedCurrent(amps);
	//The power consumer must of course draw an equivalent amount of power from the providing circuit.
	PowerConsumer::SetConsumerLoadForCurrent(convertOutputCurrentToInputCurrent(amps));
}


double PowerConverter::GetChildResistance()
{
	//the resistance of the converter in the feeding circuit is the result of 
	//how much power is being drawn from the fed circuit and the voltage of the feeding one.
	double inputcurrent = PowerSource::GetCurrentPowerOutput() / PowerConsumer::GetCurrentInputVoltage();
	double currentresistance = PowerConsumer::GetCurrentInputVoltage() / inputcurrent;
	return currentresistance;
}


double PowerConverter::GetMaxOutputCurrent(bool force)
{
	//TODO: Produces incremental error. Must find way to calculate absolute, but still cheap.
	if (IsParentSwitchedIn() || (IsAutoswitchEnabled() && force))
	{
		//if we're operable, return whatever the circuit the consumer side is switched into has left over, converted into the voltage of the source side.
		//there is one problem, though. The power the converter is already drawing is, as far as the circuit is concerned, not surplus. It's being eaten by one of its consumers, after all.
		//So the power the converter is currently getting is available in addition to anything the circuit might still have left over.
		double surpluscurrent = childcircuit->GetMaximumSurplusCurrent();
		double outputcurrent = convertInputCurrentToOutputCurrent(surpluscurrent);
		
		if (outputcurrent != maxoutcurrent)
		{
			//apparently, the state in the consumer circuit has changed.
			maxoutcurrent = outputcurrent;
			RegisterChildStateChange();
		}
		return outputcurrent;
	}

	return 0;
}


double PowerConverter::convertOutputCurrentToInputCurrent(double amps)
{
	double power = amps *  PowerSource::GetCurrentOutputVoltage();
	return (power / conversionefficiency) / PowerConsumer::GetCurrentInputVoltage();
}

double PowerConverter::convertInputCurrentToOutputCurrent(double amps)
{
	double power = amps *  PowerConsumer::GetCurrentInputVoltage();
	return (power * conversionefficiency) / PowerSource::GetCurrentOutputVoltage();

}
