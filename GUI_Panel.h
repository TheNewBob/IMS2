#pragma once
class GUImanager;
class GUI_ElementStyle;

/**
 * \brief Abstraction of an entire orbiter panel.
 * Inherit this class to make your own panel implementations.
 */
class GUI_Panel
{
public:
	friend class GUImanager;
	
	/**
	 * \brief Constructs a new GUI_Panel. 
	 * \param maxWidth Maximum width of the panel, in em.
	 * \param minWidth Minimum width of the panel, in em.
	 * \param maxHeight Maximum height of the panel, in em.
	 * \param minHeight Maximum height of the panel, in em.
	 * \param viewPortWidth Width of the viewport, in pixel.
	 * \param viewPortHeight Height of the viewport, in pixel.
	 * \param panelId The orbiter panel id of the panel.
	 */
	GUI_Panel(float maxWidthEm, float minWidthEm, float heightEm, 
		int viewPortWidthPx, int viewPortHeightPx, int panelId);
	virtual ~GUI_Panel();

protected:
	int width = 0;
	int height = 0;
	int id = -1;

	/**
	 * Set the pointer to the surface handle to be used as panel background.
	 * Panels are usually rather large in VRAM, and you never display more than one at a time.
	 * It's therefore best practice to have panel backgrounds be static per module (i.e. every vessel class
	 * shares the same background of a panel). However, if the background surface would be a static member of
	 * this class, ALL types of panels would share the same background. So the surface should be a static member
	 * of the class inheriting this one, and pass a pointer to the created surface down so the base class can
	 * access the panel.
	 */
	void setSurfaceHandle(SURFHANDLE *surfhandle);

	/**
	 * Override to register panel elements for the panel.
	 * \return Flags that determine the panels scrolling behavior.
	 * \see oapi documentation of VESSEL3::SetPanelBackground() for an explanation of scrollflags.
	 */
	virtual DWORD registerPanelElements(GUImanager *gui, PANELHANDLE panelHandle, VESSEL3 *vessel) = 0;
	
	/**
 	 * Redraws the entire panel, including the panel background.
	 * Gets called automatically if the style of the panel changes.
	 * \note This will trigger a redraw of the entire panel with all areas,
	 * so make sure you have a good reason when invoking this yourself.
	 */
	void redrawPanel(GUImanager *gui, GUI_ElementStyle * style);

	
private:
	bool loadPanel(GUImanager *gui, PANELHANDLE panelHandle,
				   MESHHANDLE hPanelMesh, GUI_ElementStyle *style, VESSEL3 *vessel);
	SURFHANDLE *srf = NULL;
	static GUI_ElementStyle *style;
};

