#pragma once
class GUI_LabelValuePairState :
	public GUI_BaseElementState
{
public:
	GUI_LabelValuePairState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_LabelValuePairState() {};

	virtual string GetValue() { return value; };
	virtual bool GetHilighted() { return hilighted; };

	virtual void SetValue(string value, bool hilighted)
	{
		if (this->value != value || this->hilighted != hilighted)
		{
			this->value = value;
			this->hilighted = hilighted;
			propagateStateChange();
		}
	};

protected:
	string value = "";
	bool hilighted = false;
};