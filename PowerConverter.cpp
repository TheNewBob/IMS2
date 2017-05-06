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
							   bool global = false)
	: PowerSource(minvoltage, maxvoltage, maxpower, internalresistance, location_id, global),
	  PowerConsumer(minvoltage, maxvoltage, maxpower, location_id, 0, 0, global),
	  conversionefficiency(conversionefficiency)
{

}

PowerConverter::~PowerConverter()
{

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


void PowerConverter::Evaluate(double deltatime)
{

}


double PowerConverter::GetChildResistance()
{

}


double PowerConverter::GetMaxOutputCurrent(bool force)
{
	if (IsParentSwitchedIn() || (IsAutoswitchEnabled() && force))
	{
		//if we're operable, return whatever the circuit the consumer side is switched into has left over.
		return childcircuit->GetMaximumSurplusCurrent();
	}

	return 0;
}
