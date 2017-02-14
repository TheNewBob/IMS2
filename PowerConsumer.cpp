#include "Common.h"
#include "PowerTypes.h"
#include "PowerChild.h"
#include "PowerConsumer.h"
#include "PowerParent.h"


PowerConsumer::PowerConsumer(double minvoltage, double maxvoltage, double maxpower)
	: PowerChild(PCT_CONSUMER, minvoltage, maxvoltage), maxpowerconsumption(maxpower)
{

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
	return consumerload * maxpowerconsumption;
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

void PowerConsumer::SetConsumerLoad(double load)
{
	if (load != consumerload)
	{
		consumerload = load;
		calculateNewProperties();
	}
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

bool PowerConsumer::CanConnectToParent(PowerParent *parent)
{
	//consumers can only directly connect to a bus, and they can only have one parent
	if (parent->GetParentType() == POWERPARENT_TYPE::PPT_BUS &&
		parents.size() < 1)
	{
		return true;
	}
	return false;
}

double PowerConsumer::GetChildResistance()
{
	return consumerresistance;
}