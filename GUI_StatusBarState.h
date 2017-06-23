#pragma once
class GUI_StatusBarState :
	public GUI_BaseElementState
{
public:
	GUI_StatusBarState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_StatusBarState() {};

	virtual double GetFillStatus() { return fillstatus; };
	virtual void SetFillStatus(double fillstatus)
	{
		if (this->fillstatus != fillstatus)
		{
			this->fillstatus = fillstatus;
			propagateStateChange();
		}
	};


	virtual double GetMaxCapacity() { return maxcapacity; };
	virtual void SetMaxCapacity(double maxcapacity)
	{
		if (this->maxcapacity != maxcapacity)
		{
			this->maxcapacity= maxcapacity;
			propagateStateChange();
		}
	};

	virtual string GetUnitString() { return unitstring; };
	virtual void SetUnitString(string unitstring)
	{
		if (this->unitstring != unitstring)
		{
			this->unitstring = unitstring;
			propagateStateChange();
		}
	};

	virtual bool GetVerbose() { return verbose; };
	virtual void SetVerbose(bool verbose)
	{
		if (this->verbose != verbose)
		{
			this->verbose = verbose;
			propagateStateChange();
		}
	};

	virtual bool GetUnitScaling() { return unitscaling; };
	virtual void SetUnitScaling(bool unitscaling)
	{
		if (this->unitscaling != unitscaling)
		{
			this->unitscaling = unitscaling;
			propagateStateChange();
		}
	};


protected:
	double fillstatus = 1.0;									//!< How much the bar is filled: >= 0 <= 1
	double maxcapacity = 100;									//!< the maximum amount of whatever the statusbar represents.
	string unitstring = "%";									//!< the unit of whatever the statusbar represents. default: %
	bool verbose = false;
	bool unitscaling = false;

};