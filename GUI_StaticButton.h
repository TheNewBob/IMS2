#pragma once

/**
 * \brief A button with a static background from a texture surface.
 *
 * Acts like a button, but draws what's in the defined place on the defined surface.
 * No labeling or styling possible. 
 * \note More or less deprecated. You should have a really good reason to use this!
 */
class GUI_StaticButton :
	public GUI_BaseElement
{
public:
	/**
	 * \param _rect Parent-relative rectangle of this element
	 * \param srcx X coordinate of the bitmap on the source surface
	 * \param srcy Y coordinate of the bitmap on the source surface
	 * \param _src The source surface to take the bitmap from
	 * \param _id Locally unique identifier for this element
	 */
	GUI_StaticButton(RECT _rect, int srcx, int srcy, SURFHANDLE _src, int _id);
	~GUI_StaticButton(void);

protected:
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	RECT srcRect;							//!< Stores the rectangle of the buttons bitmap on the source surface
};
