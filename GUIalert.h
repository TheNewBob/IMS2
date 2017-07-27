#pragma once



enum
{
	ALERTTEXT = 2,
	ALERTOKBTN,
	ALERTCANCELBTN
};

/**
 * \brief Special plugin to display a popup message with an ok prompt.
 *
 * Not intended to be instantiated directly. Rather, call GUImanager::Alert()
 */
class GUIalert :
	public GUIpopup
{
public:
	/**
	 * \brief Creates a GUI alert and inserts it into the desired place in the GUI.
	 * 
	 * The Alert will automatically be destroyed when the "ok" button is cclicked.
	 * \param text The text you want to display in the alert.
	 * \param parent Pointer to the element in which the alert should be drawn
	 * \param rect parent-relative rectangle of the Alert.
	 * \param styleid Identifier for the style in which to render the alert
	 */
	GUIalert(string text, GUI_BaseElement *parent, RECT rect, string styleid);
	~GUIalert();

	/**
	 * \brief forwards a mouse event to all children.
	 * \param _event The event to be forwarded
	 * \param _x the x coordinate of the mouse click
	 * \param _y the y coordinate of the mouse click
	 * \return True if any of the children have consumed the event.
	 */
	bool ProcessChildren(GUI_MOUSE_EVENT _event, int _x, int _y);

};

