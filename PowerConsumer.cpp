#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerConsumer.h"
#include "PowerParent.h"


PowerConsumer::PowerConsumer(double minvoltage, double maxvoltage, double maxpower, UINT location_id, double standbypower, double minimumload, bool global)
	: PowerChild(PCT_CONSUMER, minvoltage, maxvoltage), maxpowerconsumption(maxpower), locationid(location_id), minimumload(minimumload), global(global)
{
	if (standbypower == -1)
	{
		this->standbypower = maxpower * 0.001;
	}
	else
	{
		this->standbypower = standbypower;
	}
//	SetConsumerLoad(0);
}


PowerConsumer::~PowerConsumer()
{

}


double PowerConsumer::GetMaxPowerConsumption()
{
	return maxpowerconsumption;
}

double PowerConsumer::GetCurrentPowerConsumption()
{
	if (running)
	{
		if (consumerload > 0)
		{
			return consumerload * maxpowerconsumption;
		}
		else
		{
			return standbypower;
		}
	}
	else
	{
		return 0;
	}
}

bool PowerConsumer::IsRunning()
{
	return running;
}


void PowerConsumer::SetRunning(bool running)
{
	if (running != this->running)
	{
		this->running = running;
		calculateNewProperties();
	}
}


double PowerConsumer::GetInputCurrent()
{
	return consumercurrent;
}


double PowerConsumer::GetConsumerLoad()
{
	return consumerload;
}


double PowerConsumer::GetConsumerMinimumLoad()
{
	return minimumload;
}


bool PowerConsumer::SetConsumerLoad(double load)
{
	assert(load >= 0 && load <= 1 && "Somebody's trying to set an invalid load!");

	bool result = true;
	if (load != consumerload)
	{
		if (load >= minimumload)
		{
			consumerload = load;
		}
		else 
		{
			consumerload = 0;
			result = false;
		}
		calculateNewProperties();
	}
	return result;
}


bool PowerConsumer::SetConsumerLoadForCurrent(double current)
{
	double loadatcurrent = current / (maxpowerconsumption / inputvoltage.current);
	if (loadatcurrent > 1)
	{
		//the consumer can't consume this much current!
		SetConsumerLoad(1.0);
		return false;
	}

	return SetConsumerLoad(loadatcurrent);
}


void PowerConsumer::SetMaxPowerConsumption(double newconsumption)
{
	if (newconsumption != maxpowerconsumption)
	{
		maxpowerconsumption = newconsumption;
		maxconsumercurrent = maxpowerconsumption / inputvoltage.current;
		calculateNewProperties();
	}
}


void PowerConsumer::calculateNewProperties()
{
	consumercurrent = GetCurrentPowerConsumption() / inputvoltage.current;
	double current = 0;
	if (consumerload < minimumload)
	{
		//the consumer is on standby
		current = standbypower / inputvoltage.current;
	}
	else
	{
		current = maxconsumercurrent * consumerload;
	}
	consumerresistance = inputvoltage.current / current;   //a note to the confused, which will probably be future me: inputvoltage.current is current input voltage, nothig to do with... well... current.
	registerStateChangeWithParents();
}


void PowerConsumer::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	PowerChild::ConnectChildToParent(parent, bidirectional);
	inputvoltage.current = parent->GetOutputVoltageInfo().current;
	maxconsumercurrent = maxpowerconsumption / inputvoltage.current;
	calculateNewProperties();
}

void PowerConsumer::DisconnectChildFromParent(PowerParent *parent, bool bidirectional)
{
	PowerChild::DisconnectChildFromParent(parent, bidirectional);
	running = false;
	consumerload = 0;
	maxconsumercurrent = -1;
}

/*
void PowerConsumer::SetChildSwitchedIn(bool switchedin)
{
	//switch in as any child, but recalculate the current properties.
	//the consumer might be in a different circuit now, with different voltage!
	PowerChild::SetChildSwitchedIn(switchedin);
	if (switchedin)
	{
		calculateNewProperties();
	}
}
*/

bool PowerConsumer::CanConnectToParent(PowerParent *parent, bool bidirectional)
{
	//consumers can only directly connect to a bus, and they can only have one parent
	if (parent->GetParentType() == POWERPARENT_TYPE::PPT_BUS &&
		parents.size() < 1 && PowerChild::CanConnectToParent(parent, bidirectional))
	{
		return true;
	}
	return false;
}

double PowerConsumer::GetChildResistance()
{
	return consumerresistance;
}

UINT PowerConsumer::GetLocationId()
{
	return locationid;
}

bool PowerConsumer::IsGlobal()
{
	return global;
}

