#pragma once

/**
 * A checkbox. A box to the left that has two states, checked or unchecked, with a text to the right.
 * Clicking the box toggles the state. You get the picture.
 */
class GUI_CheckBox :
	public GUI_BaseElement
{
public:
	
	/**
	 * \param _text The text to be displayed at the righthand side of the checkbox
	 * \param _rect Parent-relative position and dimensions
	 * \param _id Identifier of the element, locally unique to the containing GUIentity
	 * \param _style The style to render the checkbox in
	 */
	GUI_CheckBox(string _text, RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_CheckBox();
	
	/**
	 * \return True if the box is checked, false otherwise
	 */
	bool Checked(){ return checked; };
	
	/**
	 * \brief Toggles the state of the checkbox from true to false and back
	 * \return the new state of the checkbox
	 */
	virtual bool ToggleChecked();
	
	/**
	 * \brief Sets the state of the checkbox
	 * \param _checked Pass true to check the checkbox, false to uncheck it
	 */
	virtual void SetChecked(bool _checked);


protected:
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	virtual bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief Draws the checkbox to its source surface
	 */
	void createCheckBox();

	string text;				//!< Stores the text of the checkbox
	bool checked;				//!< Stores the state of the Checkbox
};

