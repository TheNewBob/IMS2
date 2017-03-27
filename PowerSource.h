


class PowerSource : public PowerParent
{
public:

	/**
	 * \param minvoltage Lower bound of voltage range for this source.
	 * \param maxvoltage Upper bound of voltage range for this source.
	 * \param maxpower Maximum power output of this source in Watts.
	 * \param internalresistance The internal resistance of the power source in Ohm.
	 * \param location_id The identifier of the objects location. Unless both objects are global,
	 *	relationships can only be formed with objects in the same location.
	 * \param global If true, this source can form connections to other global objects regardless of their location.
	 */
	PowerSource(double minvoltage, double maxvoltage, double maxpower, double internalresistance, UINT location_id, bool global = false);
	virtual ~PowerSource();

	/**
	 * \return Max power output in Watt.
	 */
	virtual double GetMaxPowerOutput();

	/**
	 * \return Current Power Output in Watt.
	 */
	virtual double GetCurrentPowerOutput();

	/**
	 * \return The internal resistance in Ohm.
	 */
	virtual double GetInternalResistance();

	/**
	 * \return The current flowing out of the source at this moment, in Ampere.
	 */
	virtual double GetOutputCurrent();

	/**
	 * \return The maximum output current this source can proliferate.
	 * \param force If true, forces the source to reveal the current it could proliferate regardless of state (though not regardless of restrictions).
	 * \note The force argument can be confusing, as it considers state as well as user settings. It also only makes sense in inheriting classes, like rechargables.
	 *	Let's suppose a rechargable is in auto-mode, i.e. it is charging or idling when its current is not required. The normal query for available current will be with forced = false,
	 *	and will return 0. But if the system realises that it doesn't have enough power, it will query again with forced = true, and the rechargable will return its true maximum output
	 *	current, so the system knows what is truly available. If the system then requests current, the rechargable will automatically switch to feeding and provide current. If on the other hand
	 *	it was set to charge mode or switched out by the user, it will still return 0 on that second query.
	 */
	virtual double GetMaxOutputCurrent(bool force = false);

	/**
	 * Sets the current this source is outputing.
	 * \param amps Requested current in Ampere.
	 * \note ONLY call from the containing circuit!
	 */
	virtual void SetRequestedCurrent(double amps);

	/**
	 * Sets the maximum power output of this source. 
	 * Should only be used by containing classes to simulate decay and similar. Is obviously used very frequently by sources
	 * whose max output depends on environmental conditions like solar panels.
	 * \param watts The new maximum power output in Watts.
	 */
	virtual void SetMaxPowerOutput(double watts);

	//implementation of PowerParent
	virtual void Evaluate(double deltatime);

	virtual void ConnectParentToChild(PowerChild *child, bool bidirectional = true);

	virtual void SetCircuitToNull();

	/**
	 * \brief A power source can only connect to a single bus that is in the appropriate voltage range.
	 */
	virtual bool CanConnectToChild(PowerChild *child, bool bidirectional = true);

	virtual UINT GetLocationId();

	virtual bool IsGlobal();

protected:

	double maxpowerout = -1;				//!< maximum power output this source can provide in Watts.
	double maxoutcurrent = -1;			//maximum current this source can provide in Amperes.
	double internalresistance = -1;
	double curroutputcurrent = -1;

private:
	UINT locationid = 0;
	bool global = false;
};


/**
* \brief Struct used to store transient data of powersources during computations.
* This does a lot of things you would expect the actual class to do itself.
* The difference is, this data is only valid for one step in the calculation,
* and can be safely applied to the source once everything has been calculated.
*/
struct POWERSOURCE_STATS
{
	POWERSOURCE_STATS(PowerSource *source, bool force)
	{
		psource = source;
		maxcurrent = source->GetMaxOutputCurrent(force);
		baseresistance = source->GetInternalResistance();
	}

	/**
	* Helper function to automate limiting without having to set it externally
	*/
	void SetRequestedCurrent(double current)
	{
		requestedcurrent = current;
		if (requestedcurrent > maxcurrent)
		{
			limited = true;
			deliveredcurrent = maxcurrent;
		}
		else
		{
			deliveredcurrent = requestedcurrent;
		}
	}

	/**
	* Applies the set state to the powersource.
	*/
	void Apply()
	{
		psource->SetRequestedCurrent(requestedcurrent);
	}

	PowerSource *psource = NULL;			//!< pointer to the powersource these stats are for.
	double maxcurrent = 0;					//!< maximum current this powersource can provide.
	double baseresistance = 0;				//!< internal resistance of this powersource.
	double requestedcurrent = 0;			//!< current requested from this powersource.
	double deliveredcurrent = 0;			//!< The current this source actually delivers
	bool limited = false;					//!< indicates if this source is limiting its current to prevent voltage drop.
};
