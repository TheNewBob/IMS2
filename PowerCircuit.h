#pragma once

class PowerSource;
class PowerBus;
class PowerParent;
struct POWERSOURCE_STATS;



class PowerCircuit
{

public:
	PowerCircuit(double voltage, PowerBus *initialbus);
	virtual ~PowerCircuit();

	/**
	 * \brief Adds a generic powerparent to the circuit.
	 * Convenience function. Evaluates what type the parent is, and calls the appropriate method.
	 * \param parent The PowerParent to be added to the circuit.
	 */
	void AddPowerParent(PowerParent *parent);
	
	/**
	 * \brief Adds a new power source to the circuit.
	 * \param source The new source to be added.
	 * \note This should not be called directly, it is called when conneciing a power source to a bus.
	 */
	void AddPowerSource(PowerSource *source);

	/**
	 * \brief Adds a new bus to the circuit.
	 * \param bus The bus to be added.
	 * \note This should not be called directly, it is called when a bus gets connected to another bus that is already part of a circuit.
	 */
	void AddPowerBus(PowerBus *bus);

	/**
	 * \brief Removes a power source from the circuit and clears its pointer to it.
	 * \param source The powerSource to remove from the circuit.
	 */
	void RemovePowerSource(PowerSource *source);

	/**
	* \brief Removes a power bus from the circuit and clears its pointer to it.
	* \param bus The PowerBus to remove from the circuit.
	*/
	void RemovePowerBus(PowerBus *bus);

	/**
	 * \brief Sets a reference to the list of power sources in this circuit.
	 * \param OUT_sources Initialised but empty reference.
	 */
	void GetPowerSources(vector<PowerSource*> &OUT_sources);

	/**
	* \brief Sets a reference to the list of buses in this circuit.
	* \param OUT_buses Initialised but empty reference.
	*/
	void GetPowerSources(vector<PowerBus*> &OUT_buses);

	/**
	 * \brief Lets the circuit know that at least one element within it has changed state.
	 */
	void RegisterStateChange();

	/**
	 * \brief Evaluates the circuit. If there were state changes, it will be recalculated.
	 */
	void Evaluate();

protected:
	vector<PowerSource*> powersources;
	vector<PowerBus*> powerbuses;
	double voltage = -1;				//!< A circuit is always of the same voltage (parallel circuit).
	double equivalent_resistance = -1;
	double total_circuit_current = 0;
	bool statechange = true;

	/**
	 * \brief Calculates the entire equivalent resistance of this circuit.
	 * This essentially allows us to calculate how much power is drawn from which source.
	 */
	void calculateEquivalentResistance();
	
	/**
	 * \brief Distributes current draw to the available power sources based on capacity and internal resistance.
	 * \return A map mapping the required current from each power source to its index in the powersources list.
	 * \param force If true, will switch in sources that are set to auto-switch. Always pass false when calling from outside the method.
	 * \note: recursive, but with a maximum depth of 2, so don't worry about it.
	 */
	void distributeCurrentDraw(bool force = false);

	/**
	 * \return The sum of equivalent internal resistances of the passed power sources
	 */
	double getSumOfEquivalentResistances(vector<POWERSOURCE_STATS*> &involved_sources);

	/**
	 * \brief Calculates how much is drawn from each powersource and limits them to prevent voltage drop if too much is drawn.
	 * \note RECURSIVE!
	 * \param non_limited_sources A vector with stats of power sources that have not yet been limited. Pass all involved sources from outside!
	 * \param required_current The total current that needs to be provided by the sources in non_limited_sources.
	 * \param force If true, will switch in sources that are set to autoswitch.
	 */
	void calculateCurrentDraw(vector<POWERSOURCE_STATS*> non_limited_sources, double required_current, bool force = false);

	/**
	 * \brief switches in powersources on standby until are are switched in or there is enough current available.
	 * \param IN_OUT_involved_sources The newly switched in powersources are appended to this list.
	 * \param missing_current The amount of current the circuit still needs, in amps.
	 * \return The amount of current still missing after the switch in. If it's 0, everything's ok.
	 */
	double switchInPowerSourcesOnStandby(vector<POWERSOURCE_STATS*> &IN_OUT_involved_sources, double missing_current);

	/**
	 * \brief Starts switching of consumers until there is enough current available.
	 * \param missing_current The amount of current that needs to be cut, in Amps.
	 * \note Goes through buses backwards.
	 */
	void reduceCircuitCurrentBy(double missing_current);

	/**
	 * \brief Pushes the provided current from the powersources through the circuit to establish final current distribution.
	 * The general state of powersources and the circuit must already be calculated when this is called. 
	 * This merely distributes the calculated current around the buses!
	 */
	void pushCurrentThroughCircuit();
};

