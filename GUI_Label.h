#pragma once

class GUI_LabelState;
/**
 * A label is a simple text on a single line without visible borders and without event handling
 *
 * A labels text is unchangable during its lifetime.
 * \note The labels text will be horizontally and vertically centered in its rect.
 */
class GUI_Label :
	public GUI_BaseElement
{
public:
	/**
	* \param _text The text to be displayed as label
	* \param _rect Parent-relative position and dimensions
	* \param _id Identifier of the element, locally unique to the containing GUIentity
	* \param _style The style to render the checkbox in
	*/
	GUI_Label(string _text, RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_Label();

	/**
	 * \brief Sets a new text for the label.
	 *
	 * This should NOT be used to display frequently updating text, much less
	 * anything that updates by the frame.
	 * The source surface is destroyed and allocated anew, so this is actually
	 * MORE expensive than blitting dynamic text to the screen.
	 * Use it if the text is generally static, but has to change at one point
	 * or another over the lifetime of the element.
	 * \param text The new text the label should display
	 */
	void ChangeText(string text);

private:
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);

	void createLabel();					//!< Draws the label to its source surface

	/**
	* \brief Helper function to simplify casting of the state.
	*/
	GUI_LabelState *cState();

};

