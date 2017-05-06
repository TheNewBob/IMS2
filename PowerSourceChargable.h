#pragma once
class PowerSourceChargable : public PowerSource, public PowerConsumer
{
public:

	/**
	 * \param minvoltage The minimum voltage for feeding or drawing current.
	 * \param maxvoltage The minimum voltage for feeding or drawing current.
	 * \param maxdischarge The maximum discharge of the source, in watts.
	 * \param maxchargingcurrent The maximum current at which this source can be charged.
	 * \param charge The charge this chargable source can hold, in Wh.
	 * \param chargingefficiency The efficiency of the charging process (>0 <1)
	 * \param internalresistance The initial internal resistance when acting as a source.
	 * \param location_id The identifier of the objects location. Unless both objects are global,
	 *	relationships can only be formed with objects in the same location.
	 * \param minimumcharchingload The minimum load (fraction of maxchargingpower) this chargable source needs to recharge.
	 * \param global If true, this chargable source can form connections to other global objects regardless of their location.
	 */
	PowerSourceChargable(double minvoltage, 
						 double maxvoltage, 
						 double maxdischarge, 
						 double maxchargingpower, 
						 double charge,
						 double chargingefficiency,
						 double internalresistance, 
						 UINT location_id, 
						 double minimumchargingload,
						 bool global = false);

	~PowerSourceChargable();

	/**
	 * \return The maximum charge in Wh
	 */
	virtual double GetMaxCharge();

	/**
	 * \return The current charge in Wh
	 */
	virtual double GetCharge();

	/**
	 * \return The current charging efficiency.
	 */
	virtual double GetChargingEfficiency();
	
	/**
     * \brief Set the maximum charge in Wh, for example to simulate degradation and repairs.
	 * \param maxcharge New maximum charge in Wh.
	 */
	virtual void SetMaxCharge(double maxcharge);

	/**
	 * \brief Sets the current charge in Wh.
	 * \note Do no use this to simulate regular discharge! Regular discharge is handled by the object itself.
	 *	Rather, use this to simulate loss of charge in extraordinary circumstances, usually malfunction and damage related.
     */
	virtual void SetCharge(double charge);

	/**
     * \brief Set the charging efficiency, usually to simulate degradation.
	 * \param efficiency >0 <1
	 */
	virtual void SetChargingEfficiency(double efficiency);

	/**
	 * \brief Forces the source to charge itself and not provide any power.
	 * \note: This overrides prior settings to SetToProviding() and SetAutoSwitchEnabled(), 
	 *	and is overriden by successive valls to those methods.
	 */
	virtual void SetToCharging();

	/**
 	 * \brief Forces the source to provide power until empty. Even then, it won't start recharging automatically.
	 * \note This overrides prior settings to SetToCharging() and SetAutoSwitchEnabled(),
	 *	and is overriden by successive valls to those methods.
	 */
	virtual void SetToProviding();

	//implementation of PowerChild
	virtual void ConnectChildToParent(PowerParent *parent, bool bidirectional = true);

	virtual void DisconnectChildFromParent(PowerParent *parent, bool bidirectional = true);

	//implementation of PowerParent
	virtual void Evaluate(double deltatime);

	virtual void ConnectParentToChild(PowerChild *child, bool bidirectional = true);

	virtual void DisconnectParentToChild(PowerChild *child, bool bidirectional = true);

	virtual void SetParentSwitchedIn(bool switchedin);

	virtual double GetMaxOutputCurrent(bool force = false);

protected:
	double charge = -1;
	double maxcharge = -1;
	double efficiency = -1;
	double autoswitchthreshold = 0.2;					//!< Source will not be automatically switched in to provide power if charge is below this threshold.
	bool settocharging = false;							//!< if set to true, this source will attempt to charge no matter what. If set to false, autoswitch determinves the behavior. 

};

