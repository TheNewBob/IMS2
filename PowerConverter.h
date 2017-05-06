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

	
	//PowerParent implementation
	virtual bool CanConnectToChild(PowerChild *child, bool bidirectional = false);
	
	virtual void Evaluate(double deltatime);

	virtual double GetMaxOutputCurrent(bool force = false);


	//PowerChild implementation
	virtual bool CanConnectToParent(PowerParent *parent, bool bidirectional = false);

	virtual double GetChildResistance();

protected:
	double conversionefficiency = 0;
	PowerCircuit *childcircuit;
};