#include "GUI_Common.h"
#include "GUI_CheckBox.h"
#include "GUI_RadioButton.h"
#include "GUI_CheckBoxState.h"
#include "GUI_RadioButtonState.h"


GUI_RadioButton::GUI_RadioButton(string _text, RECT _rect, int _id, GUI_ElementStyle *_style)
	: GUI_CheckBox(_text, _rect, _id, _style)
{

}


GUI_RadioButton::~GUI_RadioButton()
{
}

void GUI_RadioButton::initialiseState()
{
	state = new GUI_RadioButtonState(this);
}


bool GUI_RadioButton::ToggleChecked()
{
	if (!checked)
	{
		SetChecked();
		return true;
	}
	return false;
}

void GUI_RadioButton::SetChecked(bool _checked)
{
	if (!checked)
	{
		for (UINT i = 0; i < groupedbuttons.size(); ++i)
		{
			groupedbuttons[i]->uncheck();
		}
		GUI_CheckBox::SetChecked(true);
	}
}

GUI_RadioButton *GUI_RadioButton::GetCheckedButton()
{
	if (checked)
	{
		return this;
	}
	for (UINT i = 0; i < groupedbuttons.size(); ++i)
	{
		if (groupedbuttons[i]->checked)
		{
			return groupedbuttons[i];
		}
	}
	return NULL;
}

void GUI_RadioButton::uncheck()
{
	GUI_CheckBox::SetChecked(false);
}


void GUI_RadioButton::CreateGroup(vector<GUI_RadioButton*> buttonsingroup)
{
	for (UINT i = 0; i < buttonsingroup.size(); ++i)
	{
		assert(buttonsingroup[i]->groupedbuttons.size() == 0 && "radio button is already assigned to group!");
		for (UINT j = 0; j < buttonsingroup.size(); ++j)
		{
			//obviously we don't want to add a button to itself as a grouped button
			if (j != i)
			{
				buttonsingroup[i]->groupedbuttons.push_back(buttonsingroup[j]);
			}
			//set all buttons in the group to unchecked to start
			buttonsingroup[i]->uncheck();			
		}
	}
	//check the first button in the group
	buttonsingroup[0]->SetChecked();
}


bool GUI_RadioButton::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y)
{
	if (_event == LBUP && !checked)
	{
		if (GUI_BaseElement::ProcessMe(_event, _x, _y))
		{
			ToggleChecked();
			return true;
		}
	}
	return false;
}


GUI_RadioButtonState *GUI_RadioButton::cState()
{
	return (GUI_RadioButtonState*)state;
}