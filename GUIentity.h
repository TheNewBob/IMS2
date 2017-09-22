#pragma once




class GUI_Page;
class GUI_Label;
class GUI_ListBox;
class GUI_StaticButton;
class GUI_DynamicButton;
class GUI_CheckBox;
class GUI_LabelValuePair;
class GUI_RadioButton;
class GUI_StatusBar;

/**
* \brief Abstract class providing functionality to create and manage GUI elements.
*
* The GUIentity stores GUIelements in a tree that describes a menu structure. It is capable of handling
* several such trees, so it needn't be restricted to one root. It has random access
* to all of its decendants and is a single point of entry to create new GUI elements and directly insert
* them into the proper tree using the uid of a parent.
* If elements are created dynamically (i.e. without a set uid), the GUIentity will automatically assign them
* one. UIDs of elements are LOCAL to the GUIentity managing them. Different GUIentities can have elements of
* the same UID, provided every UID only appears once in every GUIentity.
*/
class GUIentity
{
public:
	GUIentity();
	virtual ~GUIentity();

	/**
	 * \brief Registers aGUI Page without a parent, i.e. a root element.
	 * \note Only used by surfaces to register direct decendants.
	 */
	void RegisterGuiRootPage(GUI_Page * rootpage);
	
	/**
	 * \brief Registers a GUI element that has not been created by this GUIentity. 
	 *
	 * This is commonly used by specific submenu classes or other non-standard 
	 * GUI elements for which the GUIentity doesn't have a factory.
	 * \note The GUIentity takes OWNERSHIP of the passed pointer and will deallocate it on destruction.
	 * \note Destroying a registered element yourself WILL lead to mayhem!
	 * \param _element A pointer to the element to be registered
	 * \param _parentId: The UID of the parent the registered element should become a child of
	 */
	void RegisterGuiElement(GUI_BaseElement *_element, int _parentId);

	/**
	 * \return a new id unique to this GUIentity 
	 */
	int GetDynamicUID();

	/**
	 * \return the GUI_Element with the passed id, NULL if not found
	*/
	virtual GUI_BaseElement *GetElementById(int _id);

	
	//element factories*****************************************************************************


	/**
	 * \brief Creates a new GUI_Page element
	 * \param _rect The drawing rectangle the page occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent The locally unique identifier of the parent this element should become a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId: The identifier of the Style this element should be drawn in
	 * \param drawbackground If true, the page will draw a background in the passed style. 
	 * \note drawbackground: The page by default will not draw anything and only serve as a grouping element. In most cases,
	 *	this is perfectly sufficient. You should only draw the background on a page if you want the page itself to draw a border,
	 *	or a fill color that's different from the default.
	 * \return A pointer to the newly created GUI_Page.
	 * \see GUI_Page
	 */
	GUI_Page *CreatePage(RECT _rect, int parent, int _id = -1, string styleId = "", bool drawbackground = false);

	/**
	 * \brief Creates a new GUI_Page element with it position, dimensions and style defined by data received from a layout.
	 */
	GUI_Page *CreatePage(LAYOUTDATA &layoutdata, int parent, int _id = -1, bool drawbackground = false);
	
	/**
	 * \brief Creates a new GUI_Label element
	 * \param text The text being displayed on the label 
	 * \param _rect The drawing rectangle the page occupies in its parent (i.e. relative to the top-left of the parent)
     * \param parent The locally unique identifier of the parent this element should becoe a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId The identifier of the Style this element should be drawn in
	 * \return A pointer to the newly created GUI_Label
	 * \see GUI_Label
	 */
	GUI_Label *CreateLabel(string text, RECT _rect, int parent, int _id = -1, string styleId = "");

	/**
	 * \brief Creates a new GUI_Label element with it position, dimensions and style defined by by data received from a layout.
	 */
	GUI_Label *CreateLabel(LAYOUTDATA &layoutdata, string text, int parent, int _id = -1);

	/**
	 * \brief Creates a new GUI_LabelValuePair
	 * \param label The text being displayed on the label
	 * \param value The text being displayed as value
	 * \param _rect The drawing rectangle the element occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent The locally unique identifier of the parent this element should become a child of.
	 * \param id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId The identifier of the Style this element should be drawn in
	 * \param valueFontId: Lets you choose an alternate font to render the value in
	 * \return A pointer to the newly created GUI_LabelValuePair
	 * \see GUI_LabelValuePair
	 */
	GUI_LabelValuePair *CreateLabelValuePair(string label, string value, RECT _rect, int parent, int id = -1, 
		string styleId = "");

	/**
 	 * \brief Creates a new GUI_LabelValuePair element with its position, dimensions and style defined by data received from a layout.
 	 */
	GUI_LabelValuePair *CreateLabelValuePair(LAYOUTDATA &layoutdata, string label, string value, int parent, int id = -1);

	/**
	 * \brief Creates a new GUI_ListBox.
 	 * \param _rect The drawing rectangle the page occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent The locally unique identifier of the parent this element should becoe a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId: The identifier of the Style this element should be drawn in
	 * \param scrollbar_styleId: The style the scrollbar should be drawn in
	 * \param _multiselect Pass true if the listbox should handle multi-select
	 * \param _noSelect Pass true if the listbox is just intended to list things, without the capability to actually select anything
	 * \return Pointer to the newly created GUI_ListBox
	 * \see GUI_ListBox
	 */
	GUI_ListBox *CreateListBox(RECT _rect, int parent, int _id = -1,
		string styleId = "", bool _multiselect = false, bool _noSelect = false);

	/**
 	 * \brief Creates a new GUI_CreateListBox element with its position, dimensions and style defined by data received from a layout.
	 */
	GUI_ListBox *CreateListBox(LAYOUTDATA &layoutdata, int parent, int _id = -1,
		bool _multiselect = false, bool _noSelect = false);

	/**
	 * \DEPRECATED
	 * \brief Creates a new GUI_StaticButton.
	 * \param _rect The drawing rectangle the page occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param srcX The coordinate of the lefthand edge on the texture the element takes its image from
	 * \param srcY The coordinate of the top edge on the texture the element takes its image from
	 * \param _src Handle to the texture surface the element takes its image from
	 * \param parent The locally unique identifier of the parent this element should becoe a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \return Pointer to the newly created GUI_StaticButton
	 * \see GUI_StaticButton
	 */
//	GUI_StaticButton *CreateStaticButton(RECT _rect, int srcX, int srcY, SURFHANDLE _src, int parent, int _id = -1);

	/**
	 * \brief Creates a new GUI_DynamicButton
	 * \param text The text that should be displayed on the button
	 * \param _rect the drawing rectangle the page occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent: The locally unique identifier of the parent this element should becoe a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId The identifier of the Style this element should be drawn in
	 * \return A pointer to the newly created GUI_DynamicButton
	*/
	GUI_DynamicButton *CreateDynamicButton(string text, RECT _rect, int parent, int _id = -1, string styleId = "");

	/**
	 * \brief Creates a new GUI_DynamicButton element with its position, dimensions and style defined by data received from a layout.
	 */
	GUI_DynamicButton *CreateDynamicButton(LAYOUTDATA &layoutdata, string text, int parent, int _id = -1);

	/**
	 * \brief Creates a new GUI_CheckBox.
	 * \param text The text to be displayed next to the checkbox
	 * \param _rect The drawing rectangle the box (and its text) occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent The locally unique identifier of the parent this element should becoe a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId The identifier of the Style this element should be drawn in
	 * \return A pointer to the newly created GUI_CheckBox
	 * \see GUI_CheckBox
	 */
	GUI_CheckBox *CreateCheckBox(string text, RECT _rect, int parent, int _id = -1, string styleId = "");

	/**
	 * \brief Creates a new GUI_CreateCheckBox element with its position, dimensions and style defined by data received from a layout.
	 */
	GUI_CheckBox *CreateCheckBox(LAYOUTDATA &layoutdata, string text, int parent, int _id = -1);

	/**
	* \brief Creates a new GUI_RadioButton.
	* \param text The text to be displayed next to the button
	* \param _rect The drawing rectangle the button (and its text) occupies in its parent (i.e. relative to the top-left of the parent)
	* \param parent The locally unique identifier of the parent this element should become a child of.
	* \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	* \param styleId The identifier of the Style this element should be drawn in
	* \return A pointer to the newly created GUI_RRadioButton
	* \see GUI_RadioButton
	*/
	GUI_RadioButton *CreateRadioButton(string text, RECT _rect, int parent, int _id = -1, string styleId = "");

	/**
	 * \brief Creates a new GUI_RadioButton element with its position, dimensions and style defined by data received from a layout.
	 */
	GUI_RadioButton *CreateRadioButton(LAYOUTDATA &layoutdata, string text, int parent, int _id = -1);

	/**
	 * \brief Creates a GUI_StatusBar element.
	 * \param _rect _rect The drawing rectangle the status bar occupies in its parent (i.e. relative to the top-left of the parent)
	 * \param parent The locally unique identifier of the parent this element should become a child of.
	 * \param _id A locally unique identifier for this element. Pass -1 (default) to let the GUIentity assign an id itself
	 * \param styleId The identifier of the Style this element should be drawn in
	 * \return A pointer to the newly created GUI_StatusBar
	 * \see GUI_RadioButton
	 */
	GUI_StatusBar *CreateStatusBar(RECT _rect, int parent, int _id = -1, string styleId = "");

	/**
	 * \brief Creates a new GUI_StatusBar element with its position, dimensions and style defined by data received from a layout.
	 */
	GUI_StatusBar *CreateStatusBar(LAYOUTDATA &layoutdata, int parent, int _id = -1);


	/**
	 * \brief Sets the styleset this GUIentity uses.
	 * \param _styleset A string identifying the desired StyleSet
	 */
	void SetStyleSet(string _styleset);

	/**
	 * \return The id of the currently active styleset.
	 */
	string GetStyleSet();

	/** 
	 * \return A pointer to the style matching the passed id
	*/
	GUI_ElementStyle *GetStyle(string _styleId = STYLE_DEFAULT);

	/** 
	 * \return A pointer to the font matching the passed id
	*/
	GUI_font *GetFont(string _fontid);

	/**
	 * \brief checks if any children had an update NOT triggered by user input.
	 *
	 * redraws the panel area if one did.
	 * should be called every frame.
	 * Exact behavior depends on Implementation
	 * \return True if any o the elments in this entity have requested a redraw, false otherwise
	 */
	virtual bool Update() = 0;

protected:
	
	/**
	 * \brief Adds a GUI_ElementBase to a parent.
	 *
	 * Exact behavior depends on implementation.
	 * \param child The element to be added to the parent
	 * \param parentId The locally unique identifier of the parent.
	 */
	virtual bool AddChildToParent(GUI_BaseElement *child, int parentId) = 0;			
	
	map<int, GUI_BaseElement*> elements;			//!< a searchtree containing all GUI_Elements of this entity
	UINT newuid = GUI_UID_DOMAIN;					//!< Variable used to pass out dynamic UIDs for GUI elements
	string styleset = "code";					//!< Identifier of the defualt styleset of this entity


};

