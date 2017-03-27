#pragma once
#include "PowerCircuit_Base.h"
class PowerSubCircuit :
	public PowerCircuit_Base
{
public:
	/**
	 * \brief Constructs an entire subcircuit, including all buses and powersources from startingbus upwards.
	 * \param start The parent at which to start building.
	 * \param initiatingbus The bus initiating the construction, i.e. the one that MUST NOT be a member of the subcircuit.
	 * \note Subcircuits are built breadth-first.
	 */
	PowerSubCircuit(PowerParent *start, PowerBus *initiatingbus);
	~PowerSubCircuit();

	virtual void AddPowerParent(PowerParent* parent);

	/**
	 * \brief Returns the surplus current of this subcircuit, in amperes.
	 * Since a subcircuit is a snapshot of a certain part of a circuit,
	 * It may have surplus current. This is what we're actually interested
	 * in, since all that surplus current will be flowing through the bus
	 * for which the subcircuit has been stored.
	 */
	double GetCurrentSurplus();

	void Evaluate(double deltatime);

private:
	double currentsurplus = -1;

	/**
	 * \brief builds the subcircuit. See constructor for details.
	 */
	void buildCircuit(PowerParent *start, PowerBus *initiatingbus);
};

