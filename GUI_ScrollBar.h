#pragma once

/**
 * A simple scrollbar element
 *
 * This is rarely used in code directly. The more common
 * approach is for Elements that need one to create it
 * themselves and add it to their children.
 */
class GUI_ScrollBar :
	public GUI_BaseElement
{
public:
	/**
	* \param _rect Parent-relative position and dimensions
	* \param _id Identifier of the element, locally unique to the containing GUIentity
	* \param _style The style to render the checkbox in
	*/	GUI_ScrollBar(RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_ScrollBar();

	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	
	/**
	 * \brief Sets the scrollrange of the scrollbar
	 * \param range Scrollrange in pixel.
	 */
	void SetScrollRange(int range) { scrollrange = range; };
	
	/**
	 * \brief Sets the scroll speed of the scrollbar
	 * \param speed Scrollspeed in pixel per frame
	 */
	void SetScrollSpeed(UINT speed) { scrollspeed = (int)speed; };

	/**
	 * \return the current position of the scrollbar
	 * \note scrollposition is between 0 (all the way up) and scrollrange (all the way down)
	 */
	int GetScrollPos(){ return scrollpos; };

protected:
	/**
	 * \brief Draws the scrollbar to its source surface
	 */
	void createScrollBar();
	bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	int scrollpos = 0;											//!< Stores the current scrollposition of the scrollbar
	int scrollrange = 0;										//!< Stores the scrollrange of the scrollbar
	int scrollspeed = 1;										//!< Stores the scrollspeed of the scrollbar
};

