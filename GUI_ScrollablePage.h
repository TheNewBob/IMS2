#pragma once
#include "GUI_Page.h"

/**
 * A GUI_Page that can scroll its children
 * \see GUI_Page
 */
class GUI_ScrollablePage :
	public GUI_Page
{
public:
	/**
  	 * \param mRect Parent-relative position and dimensions
	 * \param _id Identifier of the element, locally unique to the containing GUIentity
	 * \param _style The style to render the checkbox in
	 * \param _scrollbarstyle An alternate style to draw the scrollbar in
	 */
	GUI_ScrollablePage(RECT mRect, int _id, GUI_ElementStyle *_style, GUI_ElementStyle *_scrollbarstyle = NULL);
	~GUI_ScrollablePage();
	
	/**
	 * \brief Manually sets the height of the content in pixel, which influences the scrollrange.
	 *
 	 * In general, the content height will be adjusted when new elements are added.
	 * However, if children are added to a child of this that exceeds the childs height,
	 * The content height has to be set manually
     * \param cheight the height of the content 
	 */
	void SetContentHeight(int cheight);

	/**
	 * \brief Calculates the scrollrange for the scrollbar based on the children
	*/
	void CalculateScrollRange();


protected:
	GUI_ScrollBar *scrollbar;								//!< Stores a pointer to the scrollbar element
	
	virtual void DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset = 0, int yoffset = 0);
	virtual void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	virtual void AddChild(GUI_BaseElement *child);
	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);


private:
	void createScrollablePage(GUI_ElementStyle *scrollbarstyle);			//!< Draws the ScrollablePage and its children to the source surface



};

