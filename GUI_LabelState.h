#pragma once
class GUI_LabelState :
	public GUI_BaseElementState
{
public:
	GUI_LabelState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_LabelState() {};

	virtual string GetText() { return text; };
	virtual void SetText(string text)
	{
		if (this->text != text)
		{
			this->text = text;
			propagateStateChange();
		}
	};

protected:
	string text = "";

};