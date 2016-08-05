#pragma once
class GUI_StatusBar :
	public GUI_BaseElement
{
public:
	/**
     *\param _rect Parent - relative position and dimensions
	 * \param _id The UID that identifies the button in the GUI
	 * \param _style The style to render the full status bar
	 */
	GUI_StatusBar(RECT _rect, int _id, GUI_ElementStyle *_style);
	~GUI_StatusBar();

	/**
	 * \brief Sets how full the status bar currently is, as a fraction of the whole
	 * \param fraction >= 0 <= 1
	 */
	void SetFillStatusByFraction(double fraction);

	/**
	* \brief Sets how full the status bar currently is, as an absolute amount relative to the maximum capacity
	* \param amount >= 0 <= maxcapacity
	*/
	void SetFillStatusByAmount(double amount);

	/**
	 * \brief Sets the capacity and unit of the status bar.
	 * \param amount the maximum amount of whatever the statusbar represents.
	 * \param unit the unit of whatever the statusbar represents.
	 * \note By default, the status bar will show percent.
	 *	If unit is anything other than percent, high values will be scaled to something more representable.
	 */
	void SetCapacity(double amount, string unit);
	
	/**
	 * \brief sets the mode in which to represent the status
	 * \param verbose If true, fill status will be shown as x / y unit. If false, status will be shown as x unit.
	 */
	void SetVerbose(bool verbose);

	/**
	 * \brief Enables or disables unit scaling.
	 * 
	 * If unit scaling is enabled, large numbers will be calculated to readable order 
	 * of magnitudes and the unit string modified to e.g. Mm, Gm, mm etc. This will not
	 * work well with non-standard units like kg (which is already a scaled unit).
	 * If unit scaling is disabled, large numbers will be written in scientific notation (e.g. 1e9),
	 * with the unmodified unit behind it.
	 */
	void SetUnitScaling(bool scaling);

private:
	void DrawMe(SURFHANDLE _tgt, int xoffset, int yoffset, RECT &drawablerect);
	bool ProcessMe(GUI_MOUSE_EVENT _event, int _x, int _y);

	void createStatusBar();

	bool updatenextframe = false;
	double fillstatus = 1.0;									//!< How much the bar is filled: >= 0 <= 1
	double maxcapacity = 100;									//!< the maximum amount of whatever the statusbar represents.
	string unitstring = "%";									//!< the unit of whatever the statusbar represents. default: %
	bool verbose = false;
	bool unitscaling = false;

	bool updateMe();
	
	/**
	 * \brief constructs the string describing the fill status of the bar
	 * \param str Reference to a string to receive the data
	 */
	void createStatusString(string &str);
	
	/**
	 * \brief prepares the source surface with the text to be blitted on the filled part of the status bar
	 * \param str Reference to the string to be printed
	 * \note Writes directly to the dedicated area in the source surface. In other words, calling this will
	 *	overwrite anything there previously.
	 * \see prepareEmptyStatusString()
	 */
	void prepareFullStatusString(string &str);

	/**
	* \brief prepares the source surface with the text to be blitted on the empty part of the status bar
	* \param str Reference to the string to be printed
	* \note Writes directly to the dedicated area in the source surface. In other words, calling this will
	*	overwrite anything there previously.
	* \see prepareEmptyStatusString()
	*/
	void prepareEmptyStatusString(string &str);
};

