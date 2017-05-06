#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerConsumer.h"
#include "PowerParent.h"
#include "PowerSource.h"
#include "PowerSourceChargable.h"

#include "PowerBus.h"


PowerSourceChargable::PowerSourceChargable(double minvoltage,
                                           double maxvoltage,
                                           double maxdischarge,
                                           double maxchargingpower,
                                           double charge,
                                           double chargingefficiency,
                                           double internalresistance,
                                           UINT location_id,
                                           double minimumchargingload,
                                           bool global)
	: PowerSource(minvoltage, maxvoltage, maxdischarge, internalresistance, location_id, global), 
	  PowerConsumer(minvoltage, maxvoltage, maxchargingpower, location_id, 0, minimumchargingload, global),
	  maxcharge(charge), charge(charge), efficiency(chargingefficiency)
{
	//for a chargable powersource it makes sense to initialise it as providing and with autoswitch enabled.
	childswitchedin = false;
	parentautoswitch = true;
}


PowerSourceChargable::~PowerSourceChargable()
{
}


double PowerSourceChargable::GetMaxCharge()
{
	return maxcharge;
}

double PowerSourceChargable::GetCharge()
{
	return charge;
}

double PowerSourceChargable::GetChargingEfficiency()
{
	return efficiency;
}

double PowerSourceChargable::GetMaxOutputCurrent(bool force)
{
	if (charge <= 0)
	{
		//if there's no charge, there's no current.
		return 0;
	}
	else if (settocharging && !parentautoswitch)
	{
		//the user does not allow this source to provide power at the moment
		return 0;
	}
	else
	{
		//under all other circumstances, report the maximum current we can provide
		return PowerSource::GetMaxOutputCurrent(force);
	}
}

void PowerSourceChargable::SetMaxCharge(double maxcharge)
{
	assert(maxcharge >= 0 && "Attempting to set a negative maximum charge!");
	if (maxcharge != this->maxcharge)
	{
		this->maxcharge = maxcharge;
		RegisterChildStateChange();
	}
}

void PowerSourceChargable::SetCharge(double charge)
{
	assert(charge >= 0 && "Attempting to set a negative charge!");
	if (charge != this->charge)
	{
		this->charge = charge;
		RegisterChildStateChange();
	}
}

void PowerSourceChargable::SetChargingEfficiency(double efficiency)
{
	assert(efficiency > 0 && efficiency < 1 && "Efficiency must be >0 <1");
	if (efficiency != this->efficiency)
	{
		this->efficiency = efficiency;
		RegisterChildStateChange();
	}
}

void PowerSourceChargable::SetToCharging()
{
	if (!settocharging || parentautoswitch)
	{
		settocharging = true;
		parentautoswitch = false;
		SetParentSwitchedIn(false);
	}
}

void PowerSourceChargable::SetToProviding()
{
	if (settocharging || parentautoswitch)
	{
		settocharging = false;
		parentautoswitch = false;
		SetParentSwitchedIn(true);
	}
}

void PowerSourceChargable::SetParentSwitchedIn(bool switchedin)
{
	//if the source is to be switched in, it must not be set to charge, and have a charge above the threshold.
	if (switchedin && !settocharging && charge > maxcharge * autoswitchthreshold)
	{
		PowerParent::SetParentSwitchedIn(true);
		PowerChild::SetChildSwitchedIn(false);
		SetConsumerLoad(1);
	}
	//if it's to be switched out, and it's set to charging or on autoswitch, it must start to charge.
	else if (!switchedin && (settocharging || parentautoswitch))
	{
		PowerParent::SetParentSwitchedIn(false);
		PowerChild::SetChildSwitchedIn(true);
		SetConsumerLoad(1);
	}
	//otherwise, just switch out the parent.
	else if (!switchedin)
	{
		PowerParent::SetParentSwitchedIn(false);
	}
}


void PowerSourceChargable::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	assert(parent->GetParentType() == PPT_BUS && "Rechargable sources can only be connected to buses!");
	PowerSource::ConnectParentToChild((PowerBus*)parent, bidirectional);
	PowerConsumer::ConnectChildToParent((PowerBus*)parent, bidirectional);
}

void PowerSourceChargable::DisconnectChildFromParent(PowerParent *parent, bool bidirectional)
{
	PowerSource::DisconnectParentFromChild((PowerBus*)parent, bidirectional);
	PowerConsumer::DisconnectChildFromParent((PowerBus*)parent, bidirectional);
}

void PowerSourceChargable::ConnectParentToChild(PowerChild *child, bool bidirectional)
{
	assert(child->GetChildType() == PCT_BUS && "Rechargable sources can only be connected to buses!");
	PowerSource::ConnectParentToChild((PowerBus*)child, bidirectional);
	PowerConsumer::ConnectChildToParent((PowerBus*)child, bidirectional);
}

void PowerSourceChargable::DisconnectParentToChild(PowerChild *child, bool bidirectional)
{
	PowerSource::DisconnectParentFromChild((PowerBus*)child, bidirectional);
	PowerConsumer::DisconnectChildFromParent((PowerBus*)child, bidirectional);
}

void PowerSourceChargable::Evaluate(double deltatime)
{
	assert(!(IsChildSwitchedIn() && IsParentSwitchedIn() && "Chargable Source is charging and providing at the same time, something went seriously wrong!"));

	if (IsChildSwitchedIn())
	{
		//The source is charging
		if (charge < maxcharge)
		{
			double inputcharge_inWh = GetCurrentPowerConsumption() * (deltatime / MILIS_PER_HOUR) * efficiency;
			charge += inputcharge_inWh;
			if (charge >= maxcharge)
			{
				//reached maximum charge, switch the charger out.
				charge = maxcharge;
				PowerChild::SetChildSwitchedIn(false);
				RegisterChildStateChange();
			}
		}
	}
	else if (IsParentSwitchedIn())
	{
		//the source is providing power
		if (charge > 0)
		{
			double output = GetCurrentPowerOutput();
			double factor = deltatime / MILIS_PER_HOUR;
			double outputcharge_inWh = GetCurrentPowerOutput() * (deltatime / MILIS_PER_HOUR);
			charge -= outputcharge_inWh;
			if (charge <= 0)
			{
				//we're neglecting possible overdraw at high timesteps. nobody will care that the equipment was running a few minutes longer than it should have.
				charge = 0;
				SetParentSwitchedIn(false);
				RegisterChildStateChange();
			}
		}
	}
}
