#pragma once

struct LAYOUTCOLLECTION;
class GUI_Layout;


/**
 * \brief A class that is an empty GUI element used to group children.
 *
 * A GUI_Page can draw a background if you need it to, but most of the time it's not necessary
 * \note Specialised menu implementations usually inherit this class.	
 */
class GUI_Page : public GUI_BaseElement
{
public:
	/**
	* \param mRect Parent-relative position and dimensions
	* \param _id Identifier of the element, locally unique to the containing GUIentity
	* \param _style The style to render the page in, if drawbackground is true
	* \param drawbackground Pass true if you need this page to draw background and borders 
    * \note The option to draw the background cannot be set retroactively, it must be decided at creation.
	*	The page will not bother to allocate texture memory for itself if this option is not set.
	* \see ScrollablePage
    */
	GUI_Page(RECT mRect, int _id, GUI_ElementStyle *_style, bool drawbackground = false);
	
	/**
	 * Override this method if you need a page to be created after construction.
	 * This is usually the case for root elements that are children of a GUI_Surface.
	 * \param gui The gui entity that should receive the page.
	 */
	virtual void PostConstruction(GUIentity *gui) {};

	virtual ~GUI_Page(void);

	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);

	virtual void AddChild(GUI_BaseElement *child);

protected:
	bool GUI_Page::ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	bool expandwithchildren = false;									//!< If true, the page will expand its own size when a child is added that does not fit. This is merely used to avoid clipping problems. If the page is set to draw a background, there will be problems!
	/**
	 * \brief Forces the page to stretch its size to include all current children. 
	 * \note Does not resize the background in case The page is drawn with a background!
	 */
	void reSize();		

	/**
	* \return the rect (position and dimension) of the element with the corresponding layout id.
	* \param elementid The string identifier that assigns the element its place in the layout. NOT the GUI-id of the element!
	* \param layout The layout collection to read the elements rect from.
	*/
	LAYOUTDATA getLayoutDataForElement(string elementid, LAYOUTCOLLECTION *layouts);

	/**
	 * \return the rect (position and dimension) of the element with the corresponding layout id, while ignoring rows that only consist of certain fields.
	 * \param elementid The string identifier that assigns the element its place in the layout. NOT the GUI-id of the element!
	 * \param layout The layout collection to read the elements rect from.
	 * \param ignore_fields The id of fields that you want to cut from the layout. If a row contains only ignored fields, the following rows will move up.
	 */
	LAYOUTDATA getLayoutDataForElement(string elementid, LAYOUTCOLLECTION *layouts, vector<string> &ignore_fields);

	string getElementStyle(string elementid, LAYOUTCOLLECTION *layouts);

	bool drawBackground;
private: 
	/**
	 * \brief Draws the page to its source surface, if it was created with a drawable background
	 */
	virtual void createResources();
	
};


