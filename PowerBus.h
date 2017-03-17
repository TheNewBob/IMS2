#pragma once

class PowerConsumer;
class PowerCircuit;
class PowerCircuitManager;

class PowerBus : public PowerChild, public PowerParent
{
	friend class PowerCircuitManager;
public:
	/**
	 * \param voltage The voltage at which this bus is intended to operate.
	 * \param maxamps The amount of amperes this bus is designed to tolerate (regular load)
	 * \param PowerCircuitManager THe circuit manager this bus belongs to.
	 * \param location_id The identifier of the objects location. Unless both objects are global,
	 *	relationships can only be formed with objects in the same location.
	 * \note Buses are always global, i.e. any other global object can form a relationship with any bus.
	 */
	PowerBus(double voltage, double maxamps, PowerCircuitManager *circuitmanager, UINT location_id);
	virtual ~PowerBus();

	/**
	 * \return The equivalent resistance of all consumers of this bus at this point in time, in Ohm.
	 * \note Does not take into account other buses that might be connected to this bus.
	 */
	double GetEquivalentResistance();

	/**
	 * \return The total current flowing through this bus at this moment in time, in Amperes.
	 */
	double GetCurrent();

	/**
	 * \return The maximum current this bus is designed for.
	 */
	double GetMaxCurrent();

	/**
	 * \brief Sets the current flowing through this bus at this moment, in Amperes.
	 * \note ONLY call from the containing circuit!
	 */
	void SetCurrent(double amps);

	/**
	 * Sets the maximum current for this bus, in Amperes.
	 * Use to simulate degradation.
	 */
	void SetMaxCurrent(double amps);

	/**
	 * \brief Bus will attempt to reduce its current flow by shutting down consumers.
	 * \param missing_current The amount of current the bus should reduce in amps
	 * \return How much of the missing current is "left". Can be negative if current was reduced by more than was asked!
	 */
	double ReduceCurrentFlow(double missing_current);

	/**
	 * \brief Tells the bus to calculate the total current running through it.
	 * This is the last operation in circuit evaluation to be called. Should not ever be called
	 * under any other circumstances.
	 */
	void CalculateTotalCurrentFlow();

	/**
	 * \brief Lets the bus delete all its feeding subcircuits and reconstruct them anew.
	 * This is a relatively expensive operation, but is neccessary to perform when the structure
	 * of a circuit changes.
	 * \todo This could be optimised by introducing structural state observers, so only affected
	 *	subcircuits get rebuilt. It would increase complexity quite a bit, but is a feasible option
	 *	if circuit building turns out to take too long in the sim.
	 */
	void RebuildFeedingSubcircuits();


	//PowerParent implementation
	virtual void Evaluate();

	virtual bool CanConnectToChild(PowerChild *child, bool bidirectional = true);

	virtual void ConnectParentToChild(PowerChild *child, bool bidirectional = true);

	virtual void DisconnectParentFromChild(PowerChild *child, bool bidirectional = true);

	//PowerChild implementation
	virtual void ConnectChildToParent(PowerParent *parent, bool bidirectional = true);

	virtual void DisconnectChildFromParent(PowerParent *parent, bool bidirectional = true);
	
	virtual bool CanConnectToParent(PowerParent *parent, bool bidirectional = true);

	virtual double GetChildResistance();

	virtual UINT GetLocationId();

	virtual bool IsGlobal();

protected:

	double maxcurrent = -1;
	double equivalent_resistance = -1;
	double throughcurrent = -1;

	PowerCircuitManager *circuitmanager = NULL;
	vector<PowerSubCircuit*> feeding_subcircuits;				//!< The subcircuits feeding current to this bus.
	

private:
	UINT locationid = 0;
};

