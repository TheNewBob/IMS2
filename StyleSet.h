#pragma once
/**
 * \brief A StyleSet is a collection of styles and in its entirety defines a skin
 * \note A valid StyleSet must contain ALL ids defined in string and string 
 */
class StyleSet
{
	friend class GUI_Looks;

private:

	StyleSet();
	~StyleSet();

	/**
	 * \return A pointer to the querried font
	 * \param fontid The identifier of the desired font 
	 */
	GUI_font *GetFont(string fontid);

	/**
	 * \return A pointer to the querried style
	 * \param styleId The identifier of the desired style
	 */
	GUI_ElementStyle *GetStyle(string styleId);
	
	/**
	 * \brief Adds a new font to the StyleSet
	 * \param font Pointer to the already created font
	 * \param fontid Identifier font should be associated with
	 */
	void AddFont(GUI_font *font, string fontid);
	
	/**
	* \brief Adds a new style to the StyleSet
	* \param style Pointer to the already created style
	* \param styleid Identifier this style should be associated with
	*/
	void AddStyle(GUI_ElementStyle *style, string styleid);

	map<string, GUI_font*> fonts;						//!< maps identifiers to their respective fonts
	map<string, GUI_ElementStyle*> styles;

};

