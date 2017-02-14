#pragma once

class PowerChild;
class PowerCircuit;

/**
 * \brief Abstract base class for classes that can be parents in a circuits hierarchy (i.e. feed power to other instances).
 */
class PowerParent
{
	friend class PowerChild;
public:

	/**
	 * \param type The type of this parent
	 * \param minvoltage The minimum voltage at which this parent can provide power.
	 * \param maxvoltage The maximum voltage at which this parent can provide power.
	 */
	PowerParent(POWERPARENT_TYPE type, double minvoltage, double maxvoltage);
	virtual ~PowerParent();

	/**
 	 * \brief Connects this parent to a child.
	 * \param child Pointer to the child to connect this child to.
	 * \param bidirectional Pass false to prevent the function from calling ConnectChildToParent() on the child being connected. Should only be passed false from WITHIN said method!
	 * \return A PowerCircuit IF a new powercircuit was created during the operation, NULL otherwise.
	 */
	virtual PowerCircuit *ConnectParentToChild(PowerChild *child, bool bidirectional = true);

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
	virtual bool CanConnectToChild(PowerChild *child);

	/**
	 * \brief Sets a reference to this objects list of children.
	 * \param OUT_children Initialised but empty reference that will receive the child list.
	 */
	virtual void GetChildren(vector<PowerChild*> OUT_children);

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
	 * \brief Makes this parent evaluate its state and recalculate accordingly
	 */
	virtual void Evaluate() = 0;

	/**
	* \return True if this child is switched into the circuit, false if not.
	*/
	virtual bool IsParentSwitchedIn();

	/**
	* \brief Switches the child in or out of the circuit.
	* \param switchedin Pass true to switch the child in, false to switch it out.
	*/
	virtual void SetParentSwitchedIn(bool switchedin);

	/**
	 * \brief If autoswitch is on, this parent will automatically switch itself into the circuit if power is demanded by a child and the parent is currently switched out.
	 * \return true if autoswitch is enabled, false if not.
	 */
	virtual bool IsAutoswitchEnabled();

	/**
	 * \brief Enables or disables tthis parents autoswitch functionality.
	 * \param enabled Pass true to enable, false to disable autoswitch.
	 */
	virtual void SetAutoswitchEnabled(bool enabled);

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
	 * \return The circuit this parent is a member of, or NULL if it isn't in any circuit yet.
	 */
	PowerCircuit *GetCircuit();

protected:
	vector<PowerChild*> children;

	bool child_state_changed = false;
	bool parentswitchedin = true;
	bool parentautoswitch = true;

	VOLTAGE_INFO outputvoltage;

	/**
	 * \brief Registers that the state of a child has changed.
	 */
	void RegisterChildStateChange();

	PowerCircuit *circuit = NULL;			//!< The circuit this parent is a part of.

private:
	POWERPARENT_TYPE parenttype;

};

