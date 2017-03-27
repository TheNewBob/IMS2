#pragma once

class PowerChild;
class PowerCircuit;
class PowerSubCircuit;

/**
 * \brief Abstract base class for classes that can be parents in a circuits hierarchy (i.e. feed power to other instances).
 */
class PowerParent
{
	friend class PowerChild;
	friend class PowerCircuitManager;
public:

	/**
	 * \param type The type of this parent
	 * \param minvoltage The minimum voltage at which this parent can provide power.
	 * \param maxvoltage The maximum voltage at which this parent can provide power.
	 */
	PowerParent(POWERPARENT_TYPE type, double minvoltage, double maxvoltage, bool switchable = true);
	virtual ~PowerParent();

	/**
 	 * \brief Connects this parent to a child.
	 * \param child Pointer to the child to connect this child to.
	 * \param bidirectional Pass false to prevent the function from calling ConnectChildToParent() on the child being connected. Should only be passed false from WITHIN said method!
	 * \return A PowerCircuit IF a new powercircuit was created during the operation, NULL otherwise.
	 */
	virtual void ConnectParentToChild(PowerChild *child, bool bidirectional = true);

	/**
	 * \brief Disconnects this parent from a child.
	 * \param child The child to disconnect from. Must be a member of the child list!
	 * \param bidirectional Pass false to prevent the function from calling DisconnectChildToParent() on the child being disconnected. Should only be passed false from WITHIN said method!
	 */
	virtual void DisconnectParentFromChild(PowerChild *child, bool bidirectional = true);

	/**
	 * \return True if a child can connect to this parent at this time, false if not.
	 * \param child The PowerChild instance of which you want to know whether or not it can be connected to this parent.
	 */
	virtual bool CanConnectToChild(PowerChild *child, bool bidirectional = false);

	/**
	 * \brief Sets a reference to this objects list of children.
	 * \param OUT_children Initialised but empty reference that will receive the child list.
	 */
	virtual void GetChildren(vector<PowerChild*> &OUT_children);

	/**
	* \return The voltage range and current voltage of this parent.
	* \note If only the current voltage is needed, use GetCurrentOutputVoltage for better performance!
	* \see GetCurrentOutputVoltage()
	*/
	virtual VOLTAGE_INFO GetOutputVoltageInfo();

	/**
	* \return the current voltage of this parent.
	* \note this information is also contained in the return of GetOutputVoltageInfo().
	*	If only the current voltage is required, this method should be used for performance reasons.
	* \see GetOutputVoltageInfo()
	*/
	virtual double GetCurrentOutputVoltage();

	/**
	* \return The id of the location this parent is located at.
	*/
	virtual UINT GetLocationId() = 0;

	/**
	* \return Whether this parent can form connections independant of its location.
	*/
	virtual bool IsGlobal() = 0;

	/**
	 * \brief Makes this parent evaluate its state and recalculate accordingly.
	 * \param deltatime Simulation time passed since the last evaluation, in miliseconds.
	 */
	virtual void Evaluate(double deltatime) = 0;

	/**
	* \return True if this child is switched into the circuit, false if not.
	*/
	bool IsParentSwitchedIn();

	/**
	* \brief Switches the parent in or out of the circuit.
	* \param switchedin Pass true to switch the parent in, false to switch it out.
	*/
	virtual void SetParentSwitchedIn(bool switchedin);

	/**
	 * \brief If autoswitch is on, this parent will automatically switch itself into the circuit if power is demanded by a child and the parent is currently switched out.
	 * \return true if autoswitch is enabled, false if not.
	 */
	bool IsAutoswitchEnabled();

	/**
	 * \return Whether or not this parent can be switched in and out of the circuit.
	 */
	bool IsParentSwitchable();

	/**
	 * \brief Enables or disables tthis parents autoswitch functionality.
	 * \param enabled Pass true to enable, false to disable autoswitch.
	 */
	void SetAutoswitchEnabled(bool enabled);

	/**
	 * \return The type of this parent.
	 */
	POWERPARENT_TYPE GetParentType();

	/**
	 * \brief Sets the circuit of this parent to NULL.
	 * \note Only call from Remove methods in PowerCircuit.
	 */
	virtual void SetCircuitToNull();

	/**
	 * \brief Sets the circuit of a parent.
	 * \param circuit The powercircuit this parent is now a member of.
	 * \note Used internally by PowerParent and PowerCircuit, do not call!
	 */
	virtual void SetCircuit(PowerCircuit *circuit);

	/**
	 * Tells this parent that a subcircuit is containing it, enabling it to get statechange notifications.
	 */
	void RegisterContainingSubCircuit(PowerSubCircuit *subcircuit);

	/**
	* Tells this parent that a subcircuit is containing it no longer exists and shouldn't receive status updates anymore.
	*/
	void UnregisterContainingSubCircuit(PowerSubCircuit *subcircuit);


	/**
	 * \return The circuit this parent is a member of, or NULL if it isn't in any circuit yet.
	 */
	PowerCircuit *GetCircuit();

protected:
	vector<PowerChild*> children;

	bool child_state_changed = false;
	bool parentswitchedin = true;				//!< whether this parent is switched in.
	bool parentautoswitch = false;				//!< whether this parent is set to switch in and out on demand.

	VOLTAGE_INFO outputvoltage;

	/**
	 * \brief Registers that the state of a child has changed.
	 */
	void RegisterChildStateChange();

	
	PowerCircuit *circuit = NULL;			//!< The circuit this parent is a part of.
	vector<PowerSubCircuit*> containing_subcircuits;	//!< Subcircuits containing this parent.

private:
	POWERPARENT_TYPE parenttype;
	bool parentcanswitch;				//!< whether this parent can be switched at all.

};

