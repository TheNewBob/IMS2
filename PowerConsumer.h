#pragma once

class PowerParent;

class PowerConsumer : public PowerChild
{
public:
	/**
	 * \param minvoltage Lower bound of the input voltage of this consumer.
	 * \param maxvoltage Upper bound of the Input voltage of this consumer.
	 * \param maxpower Maximum power consumption of this consumer in Watts.
	 * \param location_id The identifier of the objects location. Unless both objects are global,
	 *	relationships can only be formed with objects in the same location.
	 * \param standbypower How much the consumer consumes at standby (running, but zero load). Default is 0.1% of max power.
	 * \param minimumload The minimum load at which the consumer can operate (NOT Stanby power!). 
	 * \param global If true, this consumer can form connections to other global objects regardless of their location.
	 */
	PowerConsumer(double minvoltage, double maxvoltage, double maxpower, UINT location_id, double standbypower = -1, double minimumload = 0.01, bool global = false);

	virtual ~PowerConsumer();

	/**
	 * \return The maximum power consumption of this consumer in Watts.
	 */
	double GetMaxPowerConsumption();

	/**
	 * \return The current power consumption of this consumer in Watts.
	 */
	double GetCurrentPowerConsumption();


	/**
	 * \return True if the consumer is currently running, false if not.
	 * \note that this is inherently different from IsChildSwitchedIn(). While a consumer
	 *	that is not switched in is never running, a consumer that is switched in must not
	 *	neccessarily be. It might be broken, or not receive any current.
	 */
	bool IsRunning();

	/**
	 * \brief Sets the running state of the consumer.
	 * \param running Pass false to take this consumer out of action, true to reverse the operation.
	 */
	void SetRunning(bool running);

	/**
	 * \return The current flowing through this sonsumer at this moment, in Amperes.
	 */
	double GetInputCurrent();

	/**
	 * \return The current load of the consumer as a fraction of 1 (>= 0 <= 1)
	 */
	double GetConsumerLoad();

	/**
	 * \return The minimum load at which this consumer can operate.
	 */
	double GetConsumerMinimumLoad();

	/**
	 * \brief Sets the current load of this consumer.
	 * \param load The current load, in fractions of 1 (>= 0 <= 1)
	 * \return True if the consumer is able to operate at this load, false if not. If false is returned, it indicates that the consumers load has been set to 0.
	 */
	virtual bool SetConsumerLoad(double load);

	/**
	 * \brief Sets the consumers load so it consumes a certain current.
	 * \param current the current you want to set the load for, in amperes.
	 * \return True if the load could be set for current. False if current is above the maximum consumption, or below the minimum.
	 *	If above maximum, the load will have been set to 1. If below minimum, the load will have been set to 0.
	 */
	bool SetConsumerLoadForCurrent(double current);

	/**
	 * \brief Sets the maximum power consumption of the consumer.
	 * Use to emulate degradation or similar.
	 * \note Setting the max consumption will not reset the load for equivalent current consumption.
	 *	The load will be interpreted as a fraction of the new consumption!
	 */
	void SetMaxPowerConsumption(double newconsumption);

	//PowerChild implementation
	virtual void ConnectChildToParent(PowerParent *parent, bool bidirectional = true);

	virtual bool CanConnectToParent(PowerParent *parent, bool bidirectional = true);

	virtual void DisconnectChildFromParent(PowerParent *parent, bool bidirectional = true);

	virtual double GetChildResistance();

	virtual UINT GetLocationId();

	virtual bool IsGlobal();

protected:
	double maxpowerconsumption = -1;
	double maxconsumercurrent = -1;
	double consumercurrent = -1;
	double consumerload = -1;
	double consumerresistance = -1;
	double standbypower = -1;
	double minimumload = -1;
	bool running = true;


	/**
	 * \brief recalculates the consumers resistance and power consumption and registers a statechange with the parent.
	 */
	void calculateNewProperties();

private:
	UINT locationid = 0;
	bool global = false;
};

