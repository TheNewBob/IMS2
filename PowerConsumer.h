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
	 * \param global If true, this consumer can form connections to other global objects regardless of their location.
	 */
	PowerConsumer(double minvoltage, double maxvoltage, double maxpower, UINT location_id, bool global = false);
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
	 * \brief Sets the current load of this consumer.
	 * \param load The current load, in fractions of 1 (>= 0 <= 1)
	 */
	void SetConsumerLoad(double load);

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

	virtual double GetChildResistance();

	virtual UINT GetLocationId();

	virtual bool IsGlobal();

protected:
	double maxpowerconsumption;
	double maxconsumercurrent;
	double consumercurrent;
	double consumerload;
	double consumerresistance;

	bool running = true;


	/**
	 * \brief recalculates the consumers resistance and power consumption and registers a statechange with the parent.
	 */
	void calculateNewProperties();

private:
	UINT locationid = 0;
	bool global = false;
};

