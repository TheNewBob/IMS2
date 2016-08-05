#pragma once
/**
 * \brief A StyleSet is a collection of styles and in its entirety defines a skin
 * \note A valid StyleSet must contain ALL ids defined in GUI_FONT and GUI_STYLE 
 */
class StyleSet
{
	friend class FontsAndStyles;

private:

	StyleSet();
	~StyleSet();

	/**
	 * \return A pointer to the querried font
	 * \param fontid The identifier of the desired font 
	 */
	GUI_font *GetFont(GUI_FONT fontid = GUI_SMALL_DEFAULT_FONT);

	/**
	 * \return A pointer to the querried style
	 * \param styleId The identifier of the desired style
	 */
	GUI_ElementStyle *GetStyle(GUI_STYLE styleId = STYLE_DEFAULT);
	
	/**
	 * \brief Adds a new font to the StyleSet
	 * \param font Pointer to the already created font
	 * \param fontid Identifier font should be associated with
	 */
	void AddFont(GUI_font *font, GUI_FONT fontid);
	
	/**
	* \brief Adds a new style to the StyleSet
	* \param style Pointer to the already created style
	* \param styleid Identifier this style should be associated with
	*/
	void AddStyle(GUI_ElementStyle *style, GUI_STYLE styleid);

	map<GUI_FONT, GUI_font*> fonts;						//!< maps identifiers to their respective fonts
	map<GUI_STYLE, GUI_ElementStyle*> styles;			//!< maps identifiers to their respective styles

};

