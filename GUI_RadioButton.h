#pragma once

/**
 * \brief RadioButtons are checkboxes that can be grouped together and make sure that only one member of the group is checked at any time
 */
class GUI_RadioButton :
	public GUI_CheckBox
{
public:
	GUI_RadioButton(string _text, RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_RadioButton();

	/**
	* \brief If this button is checked, toggling does nothing. If it isn't, toggling will set it to checked
	*	and uncheck the currently checked button in the group.
	* \note In a radio button, the result of this method is equivalent to calling SetChecked().
	* \return False if the button was already checked, true otherwise
	*/
	virtual bool ToggleChecked();

	/**
	* \brief Sets the checked state of the radiobutton to true if it isn't already, and unchecks
	*	all other members of the group. Does nothing if this is the checked button!
	* \note passing false doesn't do anything. It's somewhat uggly, but I had to put it in to 
	*	preserve the signature of the overriden method.
	* \note In a radio button, the result of this method is equivalent to calling ToggleChecked().
	*/
	virtual void SetChecked(bool _checked = true);

	/**
	 * Returns the checke button in the group of this button
	 */
	GUI_RadioButton *GetCheckedButton();

	/**
	 * \brief groups all buttons in the passed list together.
	 * \param buttonsingroup A list of all buttons belonging to the new group
	 * \note Creating a group will fail if any of the buttons in the group is already a member of another group!
	 * \note The first button in the list will always be checked by default after grouping.
	 */
	static void CreateGroup(vector<GUI_RadioButton*> buttonsingroup);

protected:
	vector<GUI_RadioButton*> groupedbuttons;				//!< contains pointers to all buttons grouped with this
	virtual bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

private:
	/**
	 * \brief This method is only called internally among members of a group to set their state.
	 */
	void uncheck();

};

