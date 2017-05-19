#pragma once


class PowerConverter : public PowerSource, public PowerConsumer
{
public:

	PowerConverter(double minvoltage,
				   double maxvoltage,
				   double maxpower,
				   double conversionefficiency,
		     	   double internalresistance,
		           UINT location_id,
            	   bool global = false);

	~PowerConverter();

	/**
	 * \return Conversion efficiency as a factor > 0 <= 1. Output power is Input power * efficiency.
	 */
	virtual double GetConversionEfficiency();

	/**
	 * \brief Sets the efficiency of this converter.
	 * \param efficiency A factor > 0 <= 1 
	 * \note An efficiency factor of 1 technically violates the laws of thermodynamics, but checkt out mathematically.
	 */
	virtual void SetConversionEfficiency(double efficiency);

	//PowerParent implementation
	virtual bool CanConnectToChild(PowerChild *child, bool bidirectional = false);
	
	virtual void Evaluate(double deltatime);

	virtual double GetMaxOutputCurrent(bool force = false);


	//PowerChild implementation
	virtual bool CanConnectToParent(PowerParent *parent, bool bidirectional = false);

	virtual void ConnectChildToParent(PowerParent *parent, bool bidirectional = true);

	//PowerSource Implementation
	virtual void SetRequestedCurrent(double amps);

	//PowerCOnsumer Implementtion
	virtual bool SetConsumerLoad(double load);

protected:
	double conversionefficiency = 0;
	PowerCircuit *childcircuit;

	/**
	 * \brief Converts current at output voltage to current at input voltage.
	 * \param amps The current at output voltge to be converted, in ampere.
	 * \return current at input voltage representing equivalent power.
	 */
	double convertOutputCurrentToInputCurrent(double amps);

	/**
	* \brief Converts current at input voltage to current at output voltage.
	* \param amps The current at input voltge to be converted, in ampere.
	* \return current at output voltage representing equivalent power.
	*/
	double convertInputCurrentToOutputCurrent(double amps);

};