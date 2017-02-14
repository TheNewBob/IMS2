#pragma once

class PowerParent;
class PowerCircuit;


/**
 * \brief Abstract baseclass for classes that can be children in a circuits hierarchy (i.e. be fed power to from other instances).
 */
class PowerChild
{
	friend class PowerParent;
public:
	
	/**
	 * \param type The type of this child
	 * \param minvoltage The minimum voltage required for this child to be functional.
	 * \param maxvoltage The maximum voltage at which this child can operate.
	 */
	PowerChild(POWERCHILD_TYPE type, double minvoltage, double maxvoltage);
	virtual ~PowerChild();

	/**
	 * \brief Connects this child to a parent.
	 * \param parent Pointer to the parent to connect this child to.
	 * \return A PowerCircuit IF a new powercircuit was created during the operation, NULL otherwise.
	 */
	virtual PowerCircuit *ConnectChildToParent(PowerParent *parent, bool bidirectional = true);

	/**
	 * \brief Disconnects this child from a parent.
	 * \param parent The parent to disconnect from. Must be a member of the parent list!
	 * \param bidirectional Pass false to prevent the method from calling ConnectParentToChild() on the parent being connected. Should only be passed false from WITHIN said method!
	 */
	virtual void DisconnectChildFromParent(PowerParent *parent, bool bidirectional = true);

	/**
	 * \return True if a parent can connect to this child at this time, false if not.
	 * \param parent The PowerParent instance of which you want to know whether or not it can be connected to this child.
	 * \note This method is intended to be called from CanConnectToChild() in a PowerParent. It does not check for voltage compatibility,
	 *	as that is already verified by the calling method. From the outside, use PowerParent::CanConnectToChild() to determine whether
	 *	a connection is valid.
	 * \see PowerParent::CanConnectToParent()
	 */
	virtual bool CanConnectToParent(PowerParent *parent) = 0;

	/**
	 * \brief Sets a reference to this objects list of parents.
	 * \param OUT_parents Initialised but empty reference that will receive the parent list.
 	 * \param bidirectional Pass false to prevent the method from calling DisconnectParentFromChild() on the parent being disconnected. Should only be passed false from WITHIN said method!
	*/
	virtual void GetParents(vector<PowerParent*> OUT_parents);

	/**
	 * \return The childs resistance in Ohm.
	 * \note Implementations of this method should always return the resistance of the child alone, without considering possible attached children.
	 */
	virtual double GetChildResistance() = 0;

	/**
 	 * \return The voltage range and current voltage of this child.
	 * \note If only the current voltage is needed, use GetCurrentInputVoltage for better performance!
	 * \see GetCurrentInputVoltage()
	 */
	virtual VOLTAGE_INFO GetInputVoltageInfo();

	/**
	 * \return the current voltage of this child.
	 * \note this information is also contained in the return of GetInputVoltageInfo().
	 *	If only the current voltage is required, this method should be used for performance reasons.
	 * \see GetInputVoltageInfo()
	 */
	virtual double GetCurrentInputVoltage();

	/**
	 * \return True if this child is switched into the circuit, false if not.
	 */
	virtual bool IsChildSwitchedIn();

	/**
	 * Switches the child in or out of the circuit.
	 * \param switchedin Pass true to switch the child in, false to switch it out.
	 */
	virtual void SetChildSwitchedIn(bool switchedin);


	/**
	 * \return The type of this child.
	 */
	POWERCHILD_TYPE GetChildType();

	
protected:


	vector<PowerParent*> parents;
	bool childswitchedin = true;
	VOLTAGE_INFO inputvoltage;

	/**
	 * \brief Notifies all parents that the state of this child has changed.
	 */
	void registerStateChangeWithParents();

private:
	POWERCHILD_TYPE childtype;
};

