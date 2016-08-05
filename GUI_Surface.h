/**
 * Class that embeds the IMS GUI into an Orbiter PanelElement.
 *
 * This is the class that actually receives input and redwaw events from Orbiter,
 * so every GUI element must be a child of a GUI_Surface at the root of its branch
 * \todo Derives from PanelElement, but how necessary that really is will be revealed when MFDs are implemented.
 *		It might be possible to discard that dependency.
 * \note All GUI surfaces (that is, major control elements on a panel, or "screens" if you will) must derive from this class
 * \see GUImanager
 */
class GUI_Surface : public PanelElement, public EventHandler
{
	friend class GUImanager;
public:
	/**
	 * \param _vessel The vessel this GUI_Surface runs on
	 * \param _style The style for this surface
	 * \param _panelid The orbiter id of the panel this surface is visible on
	 */
	GUI_Surface(IMS2 *_vessel, GUI_ElementStyle *_style, int _panelid);
	~GUI_Surface();

	/**
	 * \brief Creates the surfaces visuals in memory
	 * \param surfacePos Panel coordinates at which this surface should show up
	 * \param _id Locally unique identifier of the surface inside its containing GUImanager
	 */
	virtual void Init(RECT &surfacePos, int _id);
	
	/**
	 * \brief Overload in inheriting classes to do things necessary after initialising, like registering children.
	 * \note Should only be called from GUImanager::RegisterGuiSurface()
	 */
	virtual void PostInit() = 0;										

	/**
	 * \brief Receives mouse events and sends it to children
	 * \param _event The kind of mouse event
	 * \param mx cursor X position relative to this element
	 * \param my cursor Y position relative to this element
	 */
	bool ProcessMouse2D(GUI_MOUSE_EVENT _event, int mx, int my);

	/**
	 * \brief Checks all children for visual updates. 
	 * \note Should be called every frame.
	 */
	bool Update();

	/**
	 * \brief Adds a GUI element as a child of this surface
	 * \param child the element to be adopted, obviously.
	 */
	void AddChild(GUI_BaseElement *child);
	
	/**
	 * \brief Redraws the surface and all of its children
	 */
	bool Redraw2D(SURFHANDLE surf);

	/**
	 * \return The locally unique identifier of the surface inside its containing GUImanager
	 */
	int GetId();

	/**
	 * \return The id of the orbiter panel this surface is visible on
	 */
	int GetPanelId();
	
protected:
	/**
	 * \brief Redraws this surfaces background
	 * \param surf The target surface to draw on
	 */
	virtual void RedrawBackground(SURFHANDLE surf);
	
	/**
	 * \brief Redraws all this surface's children
	 * \param surf The target surface to draw on
	 */
	virtual void RedrawElements(SURFHANDLE surf);

	/**
	 * brief Creates the surface's background in video memory
	 */
	void createGuiSurface();
	
	/**
	 * \brief Sends a mouse event to all children of this surface
	 * \param _event The kind of mouse event
	 * \param _x cursor X position relative to this element
	 * \param _y cursor Y position relative to this element
	 */
	int ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	
	/**
	 * \brief Trigers a redraw if any children consumed a mouse event
	 * \param eventId The Id of the child that consumed the event, -1 if event was not consumed
	 * \return True if a redraw was triggered, false otherwise
	 */
	virtual bool ProcessMe(int eventId);

	IMS2 *vessel;								//!< The vessel this surface is a part of
	RECT rect;									//!< The rectangle of this surface on the panel
	SURFHANDLE src;								//!< Stores the surfaces backgroud visuals
	int width;									//!< width of the surface in pixel
	int height;									//!< height of the surface in pixel
	int id;										//!< The locally unique identifier of this surface inside its containing GUImanager
	int panelId;								//!< The id of the orbiter panel this surface is drawn on
	vector<GUI_BaseElement*> children;			//!< List of children of this surface
	bool isInitialised;							//!< True if this surface was initialised
	GUI_ElementStyle *style;					//!< The style this surface is drawn in
};