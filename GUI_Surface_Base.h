#pragma once

class GUImanager;

class GUI_Surface_Base
{
public:
	GUI_Surface_Base(VESSEL3 *vessel, int panelid, GUImanager *gui);
	~GUI_Surface_Base();

	/**
	 * \brief Creates the surfaces visuals in memory.
	 * \param surfacePos Panel coordinates at which this surface should show up.
	 * \param _id Locally unique identifier of the surface inside its containing GUImanager.
	 */
	void Init(RECT &surfacePos, int id);

	/**
  	 * \brief Overload in inheriting classes to do things necessary after initialising.
	 * \note Should only be called from GUImanager::RegisterGuiSurface()
	 */
	virtual void PostInit() = 0;

	
	/**
	* \brief Redraws the surface and all of its decendants.
	*/
	virtual bool Redraw2D(SURFHANDLE surf) = 0;

	/**
	* \brief Receives mouse events and sends it to decendants.
	* \param _event The kind of mouse event.
	* \param mx cursor X position relative to this element.
	* \param my cursor Y position relative to this element.
	*/
	virtual bool ProcessMouse2D(int event, int mx, int my);

	/**
	* \brief Checks all decendants for visual updates.
	* \note Should be called every frame.
	*/
	virtual bool Update() { return false; };

	/**
	 * \return The locally unique identifier of the surface inside its containing GUImanager
	 */
	int GetId() { return id; };
	
	/**
	 * \return The id of the orbiter panel this surface is visible on
	 */
	int GetPanelId() { return panelId; };

protected:
	VESSEL3 *vessel;							//!< The vessel this surface is a part of
	GUImanager *gui;							//!< The gui manager of the vessel this surface is a part of.
	RECT rect;									//!< The rectangle of this surface on the panel
	SURFHANDLE src = NULL;						//!< Stores the surfaces backgroud visuals
	int width;									//!< width of the surface in pixel
	int height;									//!< height of the surface in pixel
	int id;										//!< The locally unique identifier of this surface inside its containing GUImanager
	int panelId;								//!< The id of the orbiter panel this surface is drawn on
	bool isInitialised = false; 				//!< True if this surface was initialised
};

