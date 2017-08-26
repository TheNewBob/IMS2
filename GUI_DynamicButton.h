#pragma once

class GUI_LabelState;
 
/**
 * \brief Draws a button with definable text.
 *
 * Differs from GUI_StaticButton in that the later will draw a static image from texture.
 * \see GUI_StaticButton
 */
class GUI_DynamicButton :
	public GUI_BaseElement
{
public:
	/**
	 * \param _text The text to be displayed on the button
	 * \param _rect Parent-relative position and dimensions
	 * \param _id The UID that identifies the button in the GUI
	 * \param _style The style to render the button in
	 */
	GUI_DynamicButton(string _text, RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_DynamicButton();

private:
	
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief Draws the button to its source surface
	 */
	virtual void createResources();

	/**
	* \brief Helper function to simplify casting of the state.
	*/
	GUI_LabelState *cState();

};

