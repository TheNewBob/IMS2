

/**
 * \brief Struct to store information for blitting properties
 *
 * all coordinates are panel-relative coordinates!
 * Should only be used by the GUI internally!
 */
struct BLITDATA
{
	int targetx;			//!< left coordinate to blit to
	int targety;			//!< top coordinate to blit to
	int srcx;				//!< left coordinate to blit from
	int srcy;				//!< top coordinates to blit from
	int width;				//!< width of the rectangle to be blited
	int height;				//!< height of the rectangle to be blited

	BLITDATA()
	{
		width = 0;
		height = 0;
	};
};


#pragma once
class GUIplugin;
class GUI_BaseElementState;
class GUI_BaseElementResource;

/**
 * \brief Base class of GUI elements. All GUI elements must derive from this class.
 */
class GUI_BaseElement
{
	friend class GUIentity;
	friend class GUI_BaseElementState;
	friend class GUI_BaseElementResource;

public:
	/**
	 * \param _rect The rectangle of the element, relative to its parent.
	 * \param _id The localy unique identifier of this GUI element (unique within the scope of its GUIentity).
	 * \param _style The style the element should be rendered in
	 */
	GUI_BaseElement(RECT _rect, int _id, GUI_ElementStyle *_style);
	virtual ~GUI_BaseElement();

	/**
	 * \brief Draws the element and its children to a SURFHANDLE
	 * \param _tgt The SURFHANDLE to draw to
	 * \param drawablerect The drawable rect of the parent element (serves as clipping rect)
	 * \param xoffset The x-coordinate of the left edge of this elements rect, relative to the panel
	 * \param yoffset The y-coordinate of the top edge of this elements rect, relative to the panel
	 */
	virtual void Draw(SURFHANDLE _tgt, RECT &drawablerect, int xoffset = 0, int yoffset = 0);
	
	/**
	 * \brief Checks if this element or any of its children need to be redrawn.
	 *
	 * Gets checked every frame.
	 * Overload this method only if your element needs to
	 * update visuals at intervals without direct user input.
	 * Panel Area redraws are expensive, and there's no way to
	 * just redraw one single element!
	 * \return True if the element needs to redraw, false otherwise
	 */
	virtual bool Update();
	
	/**
	 * \brief Checks if the element or any of its children have been clicked
	 * \param _event The mouse event that triggered the call
	 * \param _x the x coordinate of the click, relative to the panel
	 * \param _y the y coordinate of the click, relative to the panel
	 * \return The id of the element that consumed the event, or -1 if no element consumed it.
	 */
	int ProcessMouse(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief Adds a child to this element
	 * \param child A pointer to the element to add as a child of this.
	 * \note It is technically possible for an element to be the child of multiple elements,
	 * but there's a high probability that it will lead to undesired side effects.
	 */
	virtual void AddChild(GUI_BaseElement *child);

	/**
	 * \brief Copies the elements rect to the passed reference
	 * \param mRect The element to receive the data
	 */
	void GetRect(RECT &mRect);

	/**
	 * \return A pointer to the style used by this element
	 */
	GUI_ElementStyle *GetStyle() { return style; };

	/**
	 * \return The width of this element in pixel
	 */
	int GetWidth() { return width; };

	/**
	 * \return The height of this element in pixel
	 */
	int GetHeight() { return height; };

	/**
	 * \return The id of this element
	 */
	int GetId();

	/**
	 * return The type of this element
	 */
	GUI_ELEMENT_TYPE GetType();

	/**
	 * \brief Sets if the elements and its children are drawn or not
	 * \param _visible Pass True to draw the element, false to hide it
	 * \note Invisible elmenets will not process events. If you want an
	 * an element to not draw, but still pprocess events, use  SetNoBlit() instead
	 * \see SetNoBlit()
	 */
	virtual void SetVisible(bool _visible);

	/** 
	 * \return True if this element is visible, false if not.
	 */
	bool GetVisible();

	/**
	* \param fixed Pass true to fix an elements position even if it is inside a scrollable element.
	*/
	virtual void SetFixed(bool fixed) { isfixed = fixed; };

	/**
	* \return True if the element has a fixed positon inside a scrollable frame (like for example scrollbars or popups)
	*/
	bool GetFixed() { return isfixed; };

	/**
	 * Sets the drawing behavior of the element without influencing its response behavior
	 * \param noblit If set to true, The element will not draw, but still process events.
	 * \see SetVisible()	
	 */
	virtual void SetNoBlit(bool noblit) { noBlitting = noblit; };

	/**
	 * \returns True if the NoBlit flag for this element was set, false otherwise
	 */
	bool GetNoBlit() { return noBlitting; };

	/**
	 * \return The SURFHANDLE on which the element has its memory image
	 */
	SURFHANDLE GetSurface(){ return src; };

	/**
	 * \brief Adds a GUIplugin as a child of this element
	 * \param plugin The plugin to add to the element.
	 */
	void AddPlugin(GUIplugin *plugin);
	
	/**
	 * \brief Unplugs a plugin from this element
	 * \param plugin The plugin to unplugg. The plugin itself will be unaffected by the operation
	 */
	void RemovePlugin(GUIplugin *plugin);

	/**
	 * \brief initiates state sharing between this element and another of same type.
	 * Two elements should share state when they are controlling the physically same thing.
	 */
	void ShareStateWith(GUI_BaseElement *who);

	/**
	 * \brief Lets this element know that an element that shared its state no longer does so, presumably because it got destroyed.
	 */
	void CancelStateSharingWith(GUI_BaseElement *who);


protected:
	GUI_ELEMENT_TYPE type;				//!< The type of this element
	RECT rect;							//!< Rect the element should draw on, relative to parent
	int width;							//!< The width of the element
	int height;							//!< The height of the element
	int id;								//!< The locally unique identifier of the element
	SURFHANDLE src = NULL;				//!< Source surface to draw the element on. When the element is drawn on the panel, it will be blitted from this surface
	bool noBlitting = false;			//!< Some elements might not have to blit themselves because they are static part of the background. This is not the same as !visible, as the element still processes events and still draws children, it just doesn't draw itself.
	vector<GUI_BaseElement*> children;	//!< List containing all direct children of this element
	vector<GUIplugin*> plugins;			//!< List containing all GUIplugins currently pluged into this element
	GUI_font *font;						//!< The font used by this element
	GUI_ElementStyle *style;			//!< The style used by this element
	bool visible;						//!< Whether the element is visible or not (if false, element and any of its children will neither draw nor process events)
	bool isfixed = false;				//!< Whether the element retains a fixed position in a scrolling frame
	bool updatenextframe = false;		//!< True if the element needs to be redrawn.

	/**
	* \brief Sets the state this element uses.
	*/
	void setState(GUI_BaseElementState *state);

	/**
	* \brief Informs this object that its state is no longer available.
	*/
	void revokeState();

	/**
	 * \brief Replaces this elements state with a new state that receives all of the old states sharers.
	 */
	void swapState(GUI_BaseElementState *newstate);


	GUI_BaseElementState *state = NULL; //!< The object defining the state of this element.


	/**
	 * \brief Draws all direct children of this element, and tells them to draw theirs
	 * \param _tgt The target surface to draw on
	 * \param drawablerect The total drawable rect of this element. Serves as clipping rect for the children
	 * \param xoffset The offset between this element rects left edge and the left panel edge, in pixel
	 * \param yoffset The offset between this element rects top edge and the top panel edge, in pixel
	 * \note Does not get called if this element is not visible.
	 */
	virtual void DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset = 0, int yoffset = 0);

	/**
	 * \brief Draws this element.
	 * 
	 * Separated from DrawChildren so it can be freely overloaded
	 * \param _tgt The target surface to draw to
	 * \param xoffset The offset between this element rects left edge and the left panel edge, in pixel
	 * \param yoffset The offset between this element rects top edge and the top panel edge, in pixel
	 * \param drawablerect The total drawable rect of this element. Serves as clipping rect.
	 */
	virtual void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);

	/**
	 * \brief Processes events for this element
	 * 
	 * Separated from ProcessChildren() so either can be freely overloaded without the other
	 * \param _event The mouse event that triggered the call
	 * \param _x the x coordinate of the click, relative to the panel
	 * \param _y the y coordinate of the click, relative to the panel
	 * \return True if this element has consumed the event
	 */
	virtual bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
 	 * \brief Forwards event to its children
	 *
	 * Separated from ProcessMe() so either can be freely overloaded without the other
	 * If you are implementing a menu as a GUI_element, overload this function to intercept
	 * the children's events.
	 * \param _event The mouse event that triggered the call
	 * \param _x the x coordinate of the click, relative to the panel
	 * \param _y the y coordinate of the click, relative to the panel
	 * \return True if this element has consumed the event
	 */
	virtual int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief used for checking whether this element needs a redraw although it didn't have any user input
	 * 
	 * Called every frame. Overload to implement "animations"
	 * \return True if the element needs a redraw, false otherwise. In baseclass, always false.
	 */
	virtual bool updateMe() { return false; };

	/**
	 * \brief checks if any of the children need a redraw
	 *
	 * Called every frame.
	 * \return True if any of the children need a redraw, false otherwise.
	 */
	virtual bool updateChildren();

	/**
	 * \brief Calculates the positional and dimensional data needed for blitting, both for source as well as target surface
	 *
	 * using the returned data for a blit operation results
	 * in the element being properly croped to not draw out
	 * of drawablerect
	 * \param xoffset The screen x coordinates at which the elments rect is placed
	 * \param yoffset The screen y coordinates at which the elments rect is placed 
	 * \param drawablerect The rect the parent allows drawing operations in
	 * \param OUT_blitdata Contains the proper blitting data after the function executes
	 */
	void calculateBlitData(int xoffset, int yoffset, RECT &drawablerect, BLITDATA &OUT_blitdata);

	/**
	 * \brief calculates the real drawable rect of this element considering the drawable rect
	 * of the parent.
	 * \param drawablerect_parent The drawable rect the element received from its parent
	 * \param xoffset The left screen coordinate of this element
	 * \param yoffset The top screen coordinate of this element
	 * \param OUT_mydrawablerect: the RECT that is to receive the real drawable rect
	 */
	void calculateMyDrawableRect(RECT &drawablerect_parent, int xoffset, int yoffset, RECT &OUT_mydrawablerect);

};