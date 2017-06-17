#pragma once
class GUI_CheckBoxState :
	public GUI_BaseElementState
{
public:
	GUI_CheckBoxState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_CheckBoxState() {};

	virtual bool GetChecked() { return checked; };
	virtual void SetChecked(bool checked)
	{
		if (this->checked != checked)
		{
			this->checked = checked;
			propagateStateChange();
		}
	};
	
protected:
	bool checked = false;
};