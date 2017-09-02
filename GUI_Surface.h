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
class GUI_Surface : public GUI_Surface_Base
{
	friend class GUImanager;
public:
	/**
	 * \param vessel The vessel this GUI_Surface runs on
	 * \param style The style for this surface
	 * \param panelid The orbiter id of the panel this surface is visible on
	 * \param rootelement A page that serves as the root element for this surface.
	 * \note A page instance passed to a Surface MUST handle its initialisation
	 * (and the creation of its children) in its PostConstruction method.
	 * \see GUI_Page::PostConstruction()
	 */
	GUI_Surface(VESSEL3 *vessel, int panelid, GUI_Page *rootelement);
	virtual ~GUI_Surface();

	virtual void PostInit();

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
	 * \brief Redraws the surface and all of its children
	 */
	bool Redraw2D(SURFHANDLE surf);

protected:

	GUI_Page *page = NULL;						//!< The page representing the root of the menu of this surface.
};