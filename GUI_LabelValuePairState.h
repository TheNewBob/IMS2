#pragma once
class GUI_LabelValuePairState :
	public GUI_BaseElementState
{
public:
	GUI_LabelValuePairState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_LabelValuePairState() {};

	virtual string GetValue() { return value; };
	virtual void SetValue(string value)
	{
		if (this->value != value)
		{
			this->value = value;
			propagateStateChange();
		}
	};

protected:
	string value = "";
};