#pragma once

/**
 * font class to handle text output
 */
class GUI_font				
{
public:
	/**
	 * \brief Creates a new font
	 * \param height The desired font height (size) in pixel
	 * \param face The typeface name of the font (e.g. "Arial")
	 * \param proportional Flag for proportional/fixed pitch font (not sure what that means, it's a sketchpad option)
	 * \param _id identifier for this font
	 * \param _color The text color of the font
	 * \param _keycolor The color of the background the font is drawn on (as we can't do color key blitting!)
	 * \param _hilightcolor An alternate color in which the font should appear when hilighted. if not defined, no hilight font will be created
	 * \param _hilightbg An alternate background color for the highlighted font. If undefined, the normal background color will be used
	 * \param style font decoration style (bold, italic, underlined etc)
	 * \note See orbiter api reference of oapiCreateFont() for more explanation on height, face, proportional and style,
	 *	as these arguments will be passed unaltered to sketchpad to draw the font.
	 * \see FontsAndStyles
	 */
	GUI_font(int height, string face, bool proportional, int _id, GUI_COLOR _color, GUI_COLOR _keycolor,
		GUI_COLOR _hilightcolor = GUI_COLOR(0, 0, 0), GUI_COLOR _hilightbg = GUI_COLOR(0,0,0), FontStyle style = FONT_NORMAL);

	~GUI_font();

	/**
	 * \brief Prints a string to the target surface at the desired coordinates
	 *
	 * where exactly the text is printed depends on tpos and vpos.
	 * \see GUI_TEXT_POS
	 * \see GUI_TEXT_VERTICAL_POS
	 * \param tgt The SURFHANDLE to draw the text on
	 * \param text Reference to a string containing the text to be drawn
	 * \param _x x coordinate relative to the target surface. Exact interpretation depends on tpos
	 * \param _y y coordinate relative to the target surface Exact interpretation depends on vpos
	 * \param elementPos A clipping rect. Any text falling outside of this rectangle won't be drawn.
	 * \param highlight If true, the text will be printed using the highlighted font
	 * \param tpos Text justification. can either be T_LEFT, T_RIGHT or T_CENTER
	 * \param vpos Vertical alignement of the text. can either be V_TOP, V_BOTTOM or V_CENTER
	 */
	void Print(SURFHANDLE tgt, std::string &text, int _x, int _y, RECT &elementPos, bool highlight = false, GUI_TEXT_POS tpos = T_LEFT, GUI_TEXT_VERTICAL_POS vpos = V_TOP);
	
	/**
	 * \return The id of this font
	 */
	int GetId();

	/**
	 * \return The text size of this font (pixel)
	 */
	int GetfHeight();
	
	/**
	 * \return The AVERAGE width of a character in this font
	 * \see GetTextWidth()
	 */
	int GetfWidth(){ return fwidth; };
	
	/**
	 * \return The total width of the passed string when printed in this font (pixel)
	 * \see GetfWidth()
	 */
	int GetTextWidth(string &text);

private:
	int id;					//!< The id of this font
	int fheight;			//!< height of a symbol (font size)
	int fwidth;				//!< average width of a symbol
	int width;				//!< Entire width of the texture used to prerender the font
	SURFHANDLE src;			//!< source surface with the prerendered font
	bool hasHighlightFont;	//!< whether or not this font has a variant for highlights
	GUI_COLOR color;		//!< The default color of this font
	GUI_COLOR keycolor;		//!< The default background color (the color the font is drawn on, since we don't have transparency)
	GUI_COLOR hilightcolor; //!< The color of this font if the text is hilighted
	GUI_COLOR hilightbg;	//!< The background color to draw the hilighted font on

	int charpos[94];		//!< stores character positions in the bitmap for non-monotype fonts
	
	/**
	 * \brief Allocates a texture surface and prerenders the font to it using a sketchpad. 
	 */
	void createFont(Font *font);
};