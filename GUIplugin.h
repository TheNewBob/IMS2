#pragma once

/**
 * \brief A GUIplugin is a GUIentity that can be a child of a GUI_BaseElement.
 *
 * They are designed to be a self-managed subbranch of a GUI that can be integrated into the normal GUI flow
 * by plugging them into a GUI element. The underlying GUImanager of the vessel has NO awareness of the plugin,
 * thus enabling the plugin to be added and removed from a vessel GUI while affecting only a single GUI element.
 * Since the GUIplugin manages its own children, it can be added and removed to a GUI without its own children
 * being affected in any way by the process.
 */



class GUIplugin :
	public GUIentity
{
public:
	GUIplugin();
	~GUIplugin();

	
	
	/**
	 * \brief Sends an event received from the containing GUI_BaseElement to its children
	 *
	 * Does NOT return the uid of the triggered element!
	 * First of all uids are local to GUIentities, so there is no guarantee
	 * that the trigered uid will refer to another element in the underlying
	 * GUIentity.
	 * Second, the underlying GUIentity isn't aware of individual elements in this
	 * plugin, so it couldn't do anything with the element even if it could
	 * reliably identify it.
	 * The underlying element will report itself as triggering element to
	 * its underlying GUIentity.
	 * \param _event The event that got fired in the GUI_Surface
	 * \param _x The x-position of the mouse click relative to the parent
	 * \param _y the y-position of the mouse click relative to the parent
	 * \return True if a child of this plugin has been clicked.
	 */
	virtual bool ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);
	
	/**
	 * \brief Draws all visible children of the GUIplugin
	 * \param _tgt The SURFHANDLE to draw to
	 * \param drawablerect The drawable rect of the parent, in screen coordinates. Serves as clipping rect
	 * \param xoffset by how much to offset the plugin from screen 0 in the x axis
	 * \param yoffset by how much to offset the plugin from screen 0 in the y axis
	 */
	void DrawChildren(SURFHANDLE _tgt, RECT &drawablerect, int xoffset = 0, int yoffset = 0);

	/**
	 * \return True if the GUIplugin is visible, false if not
	 */
	bool IsVisible() { return isvisible; };

	/**
	 * \param visible Pass True to render the plugin and its children, false to hide it.
	 */
	void SetVisible(bool visible) { isvisible = visible; };

	/**
	 * \return True if the element has a fixed positon inside a scrollable frame (like for example scrollbars or popups)
	 */
	bool IsFixed() { return isfixed; };
	
	/**
	 * \param fixed Pass true to fix an elements position even if it is inside a scrollable element.
	 */
	void SetFixed(bool fixed) { isfixed = fixed; };

	/**
	 * \return returns the first root child in this plugin that is visible (which is usually the active page),
	 * NULL if the plugin has no visible children
	 */
	GUI_BaseElement *GetFirstVisibleChild();

	/**
	 * \brief checks all children if any of them need a redraw.
	 *
	 * This is used to facilitate "animations", like blinkenlights
	 * or regularly updating text data. Basically, it is an opportunity
	 * for the Element to say "hey, I need a redraw" even though it wasn't
	 * clicked. Gets called every frame.
	 * \return True if the element needs to redraw, false otherwise.
	 */
	bool Update();

protected:

	bool isvisible = false;									//!< Tells the plugin whether to draw its elements or not
	bool isfixed = false;									//!< Whether the plugin has a fixed position inside a scrollable element
	vector<GUI_BaseElement*> rootelements;					//!< The GUImanager can assume that all its registered GUI_Surfaces are root elements. The plugin doesn't have that luxury, but still needs to know which elements are at the root.

	/**
	 * \brief Adds a child to an element in this GUIplugin
	 * child: Pointer to the GUI element to be added to a parent
	 * parentId: uid of the intended parent. pass -1 if the element is 
	 *		a root element in this plugin.
	 * \note Contrary to a GUImanager, where every element must be the child of a GUI_surface,
	 * GUIplugins can have direct decendants.
	 */
	bool AddChildToParent(GUI_BaseElement *child, int parentId);

};

