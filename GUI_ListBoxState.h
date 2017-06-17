#pragma once
class GUI_ListBoxState :
	public GUI_BaseElementState
{
	friend class GUI_ListBox;
public:
	GUI_ListBoxState(GUI_BaseElement *owner) : GUI_BaseElementState(owner) {};
	virtual ~GUI_ListBoxState() {};

	virtual bool GetSelectBox() { return selectBox; };
	virtual void SetSelectBox(bool selectbox)
	{
		if (this->selectBox != selectBox)
		{
			this->selectBox = selectbox;
			propagateStateChange();
		}
	};

	virtual bool GetnoSelect() { return noSelect; };
	virtual void SetnoSelect(bool noSelect)
	{
		if (this->noSelect != noSelect)
		{
			this->noSelect = noSelect;
			propagateStateChange();
		}
	};

	virtual void AddEntry(string entry)
	{
		entries.push_back(entry);
		propagateStateChange();
	};

	virtual void ClearEntries()
	{
		entries.clear();
		propagateStateChange();
	};

protected:
	vector<std::string> entries;							//!< List of the elements in this ListBox
	bool selectBox;											//!< Stores whether this is a multi-select box 	
	bool noSelect;											//!< Stores whether elements of this ListBox can be selected at all
};