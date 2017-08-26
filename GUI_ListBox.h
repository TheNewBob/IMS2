#pragma once

class GUI_ListBoxState;

/**
 * \brief A box diplaying a scrollable list of text elements.
 * Can be set up as just a list without selection, a select box or a multi-select box
 */
class GUI_ListBox :
	public GUI_BaseElement
{
public:
	/**
	* \param _mRect Parent-relative position and dimensions
	* \param _id Identifier of the element, locally unique to the containing GUIentity
	* \param _style The style to render the element in
	* \param _scrollbarstyle Alternate style to render the scrollbar
	* \param _selectBox Pass true to make this a multi-select box
	* \param _noSelect Pass true to disable selection by the user
	*/
	GUI_ListBox(RECT _mRect, int _id, GUI_ElementStyle *_style, bool _selectBox = false, bool _noSelect = false);
	~GUI_ListBox(void);

	/**
	 * \brief Adds a new text element to the list
	 * \param element The text of the item to be added to the list
	 * \param hilight Pass true to immediately mark the new element as selected
	 */
	void AddElement(string element, bool hilight = false);

	/**
	 * \return The index of the FIRST selected element in the list. -1 if no item is selected.
	 */
	int GetSelected();

	/**
	 * \return The text of the first selected element in the list. "" if no element is selected.
	 */
	std::string GetSelectedText();
	
	/**
	 * \brief Deletes all items in the list.
	 */
	void clear();

	/**
	 * \brief Sets a new (or additional) selected element
	 * \param _selected The index of the element to select.
	 */
	void SetSelected(int _selected);

	void Draw(SURFHANDLE surf, RECT &drawablerect, int xoffset = 0, int yoffset = 0);

	/**
	 * \brief Sets the justification of the text inside the list
	 * \param tpos Can be T_RIGHT, T_LEFT or T_CENTER
	 */
	void SetListJustification(GUI_TEXT_POS tpos);

	/**
	 * \return How many elements there are in the list
	 */
	int NumEntries();

	/**
	 * \brief Used to check whether an element is selected in multi-select boxes
	 * \param index The index of the element you want to know about.
	 * \return True if the element at the passed index is selected, false otherwise.
	 */
	bool IsHighlight(UINT index);


	/**
	 * \return The height a GUI_ListBox needs to have to display a certain number of lines in a certain style
	 * \param numlines How many lines the listbox should be able to display
	 * \param styleid Id of the style the listbox would use
	 */
	static int GetNeededListBoxHeight(int numlines, string styleid);

protected:


	bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	/**
	 * \brief Draws the ListBox and its scrollbar to its source surface
	 * \param scrollbarstyle The style in which the scrollbar should be drawn
	 */
	virtual void createResources();

	int nLines;												//!< Stores the number of lines The listbox can display without scrolling
	int selected;											//!< Stores the primary selected element (in a multi-select box, the last item to be selected).
	int lineSpace;											//!< The spacing between the lines of the ListBox, in pixel
	int scrlBarWidth;										//!< THe width of the scrollbar in pixel
	int scrlBarHeight;										//!< The height of the scrollbar in pixel
//	vector<std::string> entries;							//!< List of the elements in this ListBox
//	bool selectBox;											//!< Stores whether this is a multi-select box 	
//	bool noSelect;											//!< Stores whether elements of this ListBox can be selected at all
	GUI_ScrollBar *scrollbar;								//!< Pointer to the ListBox's scrollbar element
	
	vector<UINT> hilights;									//!< List of indices of all selected items in a multi-select box
	GUI_TEXT_POS listJustification;							//!< Stores the justification of the list text

private:
	GUI_ListBoxState *cState();
};
