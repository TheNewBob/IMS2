#pragma once


class GUI_LabelValuePairState;
/* \brief A fixed label that displays a dynamic value behind it.
 * for example: color: red
 * The label (color:) cannot be changed after creation,
 * while the value is printed dynamically and can be changed at any time
 */
class GUI_LabelValuePair :
	public GUI_BaseElement
{
public:
	/**
	 * \param _label: the text to be displayed as label
	 * \param _value: the text to be displayed as value
	 * \param _rect: the rectangle defining the position of the element relative to the parent
	 * \param _id: a locally unique identifier for this element
	 * \param _style: the style in which the element should be drawn in
	 * \param _valuefont: an alternate font for the value. if NULL, label and value will use the same font 
	 */
	GUI_LabelValuePair(string _label, string _value, RECT _rect, int _id, GUI_ElementStyle *_style, GUI_font *_valuefont = NULL);
	~GUI_LabelValuePair();

	/**
	 * \brief Sets a new value and updates the source surface with the new text
	 * \param _value The new text to be printed as value
	 * \param hilighted: whether the value should be printed in hilighted styling
	 * \note valuefont needs to support hilighting for this to have effect)
	 */
	void SetValue(string _value, bool hilighted = false);

	/**
	 * \return the current value of the element
	*/
	string GetValue();

protected:
//	string value;							//!< Stores the elements value text
	string label;							//!< Stores the elements label text
	GUI_font *valuefont;					//!< Stores the font in which the value is printed
	int labelwidth;							//!< Distance from the element rects left to the left edge of the value 
	
	/**
	 * \brief Draws the element to its source surface
	 */
	virtual void createResources();
	
	/**
	 * Reprints the value text onto the source surface
	 */
	void changeValue();
	
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);

	/**
	 * \brief Redraws the current value.
	 */
	void loadValue();

private:
	/**
	* \brief Helper function to simplify casting of the state.
	*/
	GUI_LabelValuePairState *cState();

};
