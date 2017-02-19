#pragma once

/**
 * \brief Class to manage the existing powercircuits of an object in which circuits are allowed to interact.
 * There should be no more and no less than one PowerCircuitManager instance per instance of an object that can contain multiple powercircuits.
 */
class PowerCircuitManager
{
public:
	PowerCircuitManager();
	~PowerCircuitManager();

	/**
	 * \brief Creates a new PowerCircuit.
	 * \param initialbus The bus used to initialise the circuit.
	 * \return A pointer to the newly created circuit.
	 */
	PowerCircuit *CreateCircuit(PowerBus *initialbus);
	
	/**
	 * \brief Deletes an existing PowerCircuit that contains no objects anymore.
	 * Leaves all objects associated with the deleted circuit in tact.
	 * \note Will trigger an assert if the circuit is not empty!
	 */
	void DeletePowerCircuit(PowerCircuit *circuit);
	
	/**
	 * \brief merges two PowerCircuits into one.
	 * The circuits to be merged must have the same voltage, and must both be
	 *	contained by this PowerCircuitManager.
	 * \param circuit_a The circuit to merge into. This pointer will still be valid after the operation.
	 * \param circuit_b The circuit to merge into a. This object will be destroyed in the process.
	 * \note Does automatically update the circuit pointers of objects in circuit_b to point to circuit_a.
	 */
	void MergeCircuits(PowerCircuit *circuit_a, PowerCircuit *circuit_b);

	/**
	 * \brief Evaluates all the circuits in this PowerCircuitManager.
	 */
	void Evaluate();

	/**
	 * \brief Sets the passed reference to the list of circuits in this PowerCircuitManager.
	 * \param OUT_circuits Reference to an initialised but empty vector. 
	 */
	void GetPowerCircuits(vector<PowerCircuit*> &OUT_circuits);

	/**
	 * \return The number of circuits in the manager.
	 */
	UINT GetSize();

private:
	vector<PowerCircuit*> circuits;				//!< Stores all PowerCircuits in this manager.

};

