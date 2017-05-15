#pragma once

class PowerSource;
class PowerBus;
class PowerParent;

class PowerCircuit_Base
{
public:
	PowerCircuit_Base(double voltage);
	virtual ~PowerCircuit_Base();

	/**
	* \brief Adds a generic powerparent to the circuit.
	* Convenience function. Evaluates what type the parent is, and calls the appropriate method.
	* \param parent The PowerParent to be added to the circuit.
	*/
	virtual void AddPowerParent(PowerParent *parent);

	/**
	* \brief Adds a new power source to the circuit.
	* \param source The new source to be added.
	* \note This should not be called directly, it is called when conneciing a power source to a bus.
	*/
	virtual void AddPowerSource(PowerSource *source);

	/**
	* \brief Adds a new bus to the circuit.
	* \param bus The bus to be added.
	* \note This should not be called directly, it is called when a bus gets connected to another bus that is already part of a circuit.
	*/
	virtual void AddPowerBus(PowerBus *bus);

	/**
	 * \brief Removes a powerparent from the circuit.
	 * Convenience function. Evaluates what type the parent is, and calls the appropriate method.
	 * \param parent the PowerParent to be removed from the circuit.
	 */
	virtual void RemovePowerParent(PowerParent *parent);

	/**
	* \brief Removes a power source from the circuit and clears its pointer to it.
	* \param source The powerSource to remove from the circuit.
	*/
	virtual void RemovePowerSource(PowerSource *source);

	/**
	* \brief Removes a power bus from the circuit and clears its pointer to it.
	* \param bus The PowerBus to remove from the circuit.
	*/
	virtual void RemovePowerBus(PowerBus *bus);

	/**
	* \brief Sets a reference to the list of power sources in this circuit.
	* \param OUT_sources Initialised but empty reference.
	*/
	virtual void GetPowerSources(vector<PowerSource*> &OUT_sources);

	/**
	* \brief Sets a reference to the list of buses in this circuit.
	* \param OUT_buses Initialised but empty reference.
	*/
	virtual void GetPowerBuses(vector<PowerBus*> &OUT_buses);

	/**
	 * \return The voltage of this circuit.
	 */
	virtual double GetVoltage();
	
	/**
	* \brief Lets the circuit know that at least one element within it has changed state.
	*/
	void RegisterStateChange();

	/**
	* \brief Evaluates the circuit. If there were state changes, it will be recalculated.
	* \param deltatime Simulation time passed since last evaluation, in miliseconds.
	*/
	virtual void Evaluate(double deltatime) = 0;

	/**
	* \return The number of buses and sources in this PowerCircuit.
	*/
	UINT GetSize();


protected:
	vector<PowerSource*> powersources;
	vector<PowerBus*> powerbuses;
	double voltage = -1;				//!< A circuit is always of the same voltage (parallel circuit).
	bool statechange = true;

};

