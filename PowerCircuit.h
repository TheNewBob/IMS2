#pragma once

class PowerSource;
class PowerBus;
class PowerParent;
struct POWERSOURCE_STATS;



class PowerCircuit : public PowerCircuit_Base
{
	friend class PowerParent;
	friend class PowerCircuitManager;

public:
	PowerCircuit(PowerBus *initialbus);
	virtual ~PowerCircuit();

	void AddPowerSource(PowerSource *source);

	void AddPowerBus(PowerBus *bus);

	void RemovePowerSource(PowerSource *source);

	void RemovePowerBus(PowerBus *bus);

	/**
	* \return The total current flowing through the entire circuit, in amps.
	*/
	double GetCircuitCurrent();

	/**
	* \return The equivalent resistance of the entire circuit
	*/
	double GetEquivalentResistance();

	/**
	* \brief Evaluates the circuit. If there were state changes, it will be recalculated.
	*/
	void Evaluate(double deltatime);

	/**
	 * \return The maximum surplus current this circuit has in its power sources if consumption remains constant, in amps
	 */
	double GetMaximumSurplusCurrent();

	/**
	 * \brief Informs the circuit of a change in current demand that happens DURING a systems evaluation.
	 * This only happens if the state of a circuit receiving its power from this circuit changes.
	 * \param amps Change in required current in Amperes. Can be negative, obviously.
	 */
	void RegisterCrossCircuitCurrentDemandChange(double amps);

protected:
	double equivalent_resistance = -1;
	double total_circuit_current = 0;
	bool structurechanged = false;					//shows true if the circuit structure has changed since the last evaluation.
	double circuit_current_demand_change = 0;			//shows change in current demand over an entire systems evaluation, AFTER this circuit was evaluated.

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

	/**
	 * \brief Tells all buses to reconbuild their feeding subcircuits.
	 */
	void rebuildAllSubCircuits();
};

