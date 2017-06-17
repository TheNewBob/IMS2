#pragma once
class GUI_RadioButtonState :
	public GUI_CheckBoxState
{
	friend class GUI_CheckBox;
public:
	GUI_RadioButtonState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_RadioButtonState() {};


protected:
};