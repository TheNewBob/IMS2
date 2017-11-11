#pragma once
class GUIpopup;
class GUI_Panel;

/**
 * \brief The GUI manager is a GUIentity with enhanced functionality to embedd a GUI in an orbiter vessel. 
 *
 * Every vessel using the GUI does so by carrying an instance of GUImanager.
 * The GUImanager provides functionality to register a GUIsurface with an Orbiter Panel Area, making it capable of
 * sending mouse events from orbiter to its children, and triggering panel redraws if necessary.
 */
class GUImanager : public GUIentity
{
public:
	/**
	 * \param _vessel The vessel containing this GUImanager
	 */
	GUImanager(VESSEL3 *vessel, string initialstyleset = "default");
	~GUImanager(void);

	/**
	 * \brief Registers a GUI_Surface with the GUI manager, BUT NOT YET WITH ORBITER!
	 *
	 * the reason for this is that at the time of creation, the orbiter 2dpanel 
	 * does not yet exist. Use RegisterSurfaceWithOrbiter() to register a GUI_Surface
	 * with an orbiter panel area as soon as the panel gets created!
	 * \param surf The GUI_Surface to be registered
	 * \param _id a unique identifer for the surface that will also serve to identify the panel area in orbiter
	 * \param surfacePos The absolute position this surface (and the panel area) will occupy on the panel
	 * \see RegisterSurfaceWithOrbiter()
	 */
	void RegisterGUISurface(GUI_Surface *surf, int _id, RECT surfacePos);

	/**
	 * \brief This function has to be called from clbkLoadPanel2d to register the GUI_Surface to the orbiter SURFHANDLE
	 * \param surf The GUI_Surface object to register
	 * \param hpanel PANELHANDLE to the orbiter panel this surface should be a part of
	 * \param tgt The SURFHANDLE from which this surface will receive input and redraw events
	*/
	void RegisterSurfaceWithOrbiter(GUI_Surface *surf, PANELHANDLE hpanel, SURFHANDLE tgt);

	/**
	 * \brief Registers a panel instance with this manager.
	 * The manager will take ownership of the passed pointer and will deallocate it on vessel destruction.
	 */
	void RegisterGUIPanel(GUI_Panel *panel);

	/**
	 * \return True if a panel with the passed id is registered in the manager,
	 *	false if not.
	 */
	bool IsPanelRegistered(int id);

	/**
	 * \brief Loads the panel with the passed id, if it has one.
	 * Call this method from clbkLoadPanel2D.
	 * \param id The id of the panel to load.
	 * \param hpanel The handle for this panel.
	 * \return True if the panel is registered in the manager and was loaded, false if not.
	 */
	bool LoadPanel(int id, PANELHANDLE hpanel);

	/**
	 * \return The id of the currently active orbiter panel (the panel active in the cockpit of the vessel) 
	 */
	int GetCurPanelId();

	/**
	 * \brief Notifies the GUImanager that the currently visible panel has changed
	 * \param _curPanelId The id of the now active panel
	 * \note Do not try to use this to switch the panel! This function is purely reactionary,
	 * taking note that the panel in Orbiter has ALREADY been switched!
	 */
	void SetCurPanelId(int _curPanelId);

	/**
	 * \brief Triggers a redraw of the whole currently active panel
	 */
	void RedrawCurrentPanel();
	
	/**
	 * \brief triggers a redraw of the panel area associated with a GUIsurface
	 * \param surfId the uid of the GUIsurface to be redrawn
	 */
	void RedrawGUISurface(int surfId);

	/**
	 * \brief generates events for the GUI based on events it receives from orbiter.
	 *
	 * this is necessary because orbiter frequently misses the MouseDown event, 
	 * immediately generating a MousePressed event instead.
	 * \param _event an Identifier for the GUI_event to generate (Not to be confused with the other kind of events!)
	 */
	GUI_MOUSE_EVENT GetEvent(int _event);

	/**
	 * \brief Looks for a GUI_Element with the passed id
	 * \param _id The locally unique identifier of the GUI_BaseElement you are looking for
	 * \return A Pointer to the GUI-element with a matching id, or NULL if no match was found.
	 */
	GUI_BaseElement *GetElementById(int _id);

	bool Update();

	/**
	* \brief checks whether a certain interval of time has been filled,
	* using a synchronised clock for the whole simulation.
	*
	* Panel redraws are expensive. As such, it makes sense that elements
	* in the panel that update at the same interval update at the same time.
	* For example, two elements that update every second should do so in the
	* same frame, not half a second appart from each other, because it saves 
	* resources. Check your intervals with this method
	* to facilitate that synchronicity.
	* \param interval_in_miliseconds The interval you wish to check
	* \return True if the interval_in_miliseconds has passed,
	* relative to the synchronised clock.
	*/
	static bool CheckSynchronisedInterval(int interval_in_miliseconds);

	/**
	 * \brief Pops up an alert with an ok prompt in the designated parent element
	 * \param text The text to be displayed in the alert
	 * \param parent The element the alert should appear in
	 * \param rect Drawing rectangle relative to the parent. pass 0,0,0,0 to let the function choose a sensible size depending on parent and text
	 * \param styleid Identifier for the style the alert should be drawn in, if different than normal
	 */
	static void Alert(string text, GUI_BaseElement *parent, RECT rect = _R(0,0,0,0), string styleid = STYLE_ERROR, string styleset = "default");

	static void ListDialog(string title, vector<string> &items, GUI_BaseElement *parent, bool(*callback)(int, void*), void *usrdata = NULL, RECT rect = _R(0, 0, 0, 0), string styleset = "default");


	/**
	 * \brief Clear a temporary plugin (Popup-dialogue). 
	 *
	 * Do not call by yourself! The alert will call this function
	 * by itself when it is closed! this will register the alert for deletion.
	 * The function is also called from GUImanager::prestep() with a NULL argument. If an alert has
	 * been registered for cleanup during the last frame, it will be deleted at this call.
	 */
	static void ClearPopup(GUIpopup *popup = NULL);

	/**
	 * \brief Get a style from the currently used styleset
	 */
	GUI_ElementStyle *GetStyle(string styleid);

	/**
	 * \brief Sets the currently used styleset.
	 * \note If the style changes, all elements have to redraw their resources,
	 *	so this should not be called lightly!
	 */
	void SetStyleset(string styleset);

	/**
	 * \brief returns the MESHHANDLE for the panel mesh.
	 */
	MESHHANDLE GetPanelMesh();

private:

	/**
	 * \brief Adds an element to the child list of another element.
	 *
	 * Contrary to the base call in GUIentity, the GUImanager needs to
	 * discriminate if a UID belongs to a GUI_BaseElement or a GUI_Surface
	 * \param child The element to be added
	 * \param parentId The localy unique identifier of the parent
	 * \return True if successful, false if the parent could not be located.
	 */
	bool AddChildToParent(GUI_BaseElement *child, int parentId);
	
	/**
	 * \brief Checks if a GUI_surface is registered with Orbiter
	 * \param _id The localy unique identifier of the GUI_Surface you want to know about
	 * \return True if the GUI_surface with the passed id is already registered, false if it isn't, or could not be located
	 */
	bool SurfaceIsRegistered(int _id);

	/**
	 * \param _id The localy unique identifier of the GUI_surface you want to retrieve
	 * \return A pointer to the requested GUIsurface
	 */
	GUI_Surface *GetSurfaceById(int _id);
	
	VESSEL3 *vessel;										//!< The vessel containing this GUImanager

	vector<GUI_Surface*> surfaces;						//!< stores pointers to all GUIsurfaces

	int curPanelId;										//!< Stores the id of currently active panel
	GUI_MOUSE_EVENT curEvent;							//!< The event currently being propagated through the children of this GUImanager
	map<int, GUI_Panel*> panels;						//!< The panels registered in this manager.

	//static synchronised gui clock
	static unsigned long lastclock;						//!< Remembers the time of the last frame					
	static UINT managerinstances;						//!< Keeps track of how many managerinstances are alive
	
	static map<GUIplugin*, GUI_BaseElement*> temporaryplugins;		//!< Stores temporary GUI plugins (popups) and their parents during their lifetime
	
	/**
	* \brief registeres GUImanagers that have completed their prestep operations
	*
	* Should be called by every manager AFTER they complete
	* processing the prestep operations. When all managers have 
	* called in, the frame is complete and the gui-clock is updated
	* to the new sim-time.
	*/
	static void registerPreStep();		

	MESHHANDLE hPanelMesh = NULL;                       // 2-D instrument panel mesh handle
};
