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
	this->running = running;
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
		calculateNewProperties();
	}
}


void PowerConsumer::calculateNewProperties()
{
	consumercurrent = GetCurrentPowerConsumption() / inputvoltage.current;
	consumerresistance = inputvoltage.current / consumercurrent;
	registerStateChangeWithParents();
}


void PowerConsumer::ConnectChildToParent(PowerParent *parent, bool bidirectional)
{
	PowerChild::connectChildToParent(this, parent, bidirectional);
	inputvoltage.current = parent->GetOutputVoltageInfo().current;
}

void PowerConsumer::DisconnectChildFromParent(PowerParent *parent, bool bidirectional)
{
	PowerChild::disconnectChildFromParent(this, parent, bidirectional);
}

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

