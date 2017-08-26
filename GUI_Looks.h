#pragma once

/**
 * \file GUI_Looks.h
 * IMS2 has a dynamic style managment with the aim to be able to define UI skins in external files to allow modding,
 * as well as to allow switching skins on the fly during the simulation, or choosing different skins for different 
 * vessels etc.
 * A bit of infrastructure is still missing to accomplish these goals, but what's already here is designed with it in mind.
 *
 * When working with styles and fonts, it is important not to think of something like CSS. The IMS2 interface is static in every important sense
 * of the word, as are all Game interfaces I know of: They are pre-rendered to increase performance, and the layout is hard-coded and cannot
 * be changed. What can be adjusted with styling is really just the outer skin: Colors, fonts and shapes, though the later only in the
 * very limited capacity of which corners should be rounded by how many pixel.
 *
 * Individual Fonts and styles are identified by the string and GUI_FONTS enumerations in GUI_common.h. These identifiers must be used
 * to identify every single style and font in existance, otherwise they will not be changeable by external files.
 * A StyleSet should be thought of as a skin, and is a collection of one of each of these styles and fonts. StyleSets in turn are 
 * managed by the GUI_Looks singleton, which defines all skins avalable in the simulation, and which is the only place fonts and styles
 * should be obtained by the factory functions in GUIentity
 */



/**
 * \brief Singleton that manages fonts and styles.
 *
 * Fonts and the Styles are not handled  by a GUImanager because a GUImanager exists as one instance per vessel, 
 * while this data is global for the entire simulation.
 * \see GUI_Looks.h
 */
class GUI_Looks
{
	friend class GuiXmlLoader;

public:
	
	
	/**
	 * \return The desired font 
	 * \param fontid The id identifying the font
	 * \param styleset The styleset the font should be taken from
	 */
	static GUI_font *GetFont(string fontid, string styleset = "default");

	/**
	 * \return The desired style
	 * \param styleId The identifier of the desired style
	 * \param styleset The styleset the style should be taken from
	 */
	static GUI_ElementStyle *GetStyle(string styleId, string styleset = "default");

	/**
	 * Converts a string with comma-separated channel values to a GUI_COLOR.
	 * \param strRGB A comma-separated string with RGB values (e.g. 255,255,255)
	 * \return A GUI_COLOR containing the passed color channels as numerical values.
	 */
	static GUI_COLOR StringToColor(string strRGB);

private:
	GUI_Looks();
	~GUI_Looks();

	static GUI_Looks *instance;						//!< Stores the instance of the fonts and styles manager

	static map<string, StyleSet*> stylesets;						//!< Stores the available StyleSets
	static string defaultStyle;						//!< Defines the default style

	/**
	 * \brief Creates a font and adds it to a styleset
	 * \param height The desired font height (size) in pixel
	 * \param face The typeface name of the font (e.g. "Arial")
	 * \param proportional Flag for proportional/fixed pitch font (not sure what that means, it's a sketchpad option)
	 * \param _id identifier for this font 
	 * \param _color The text color of the font
	 * \param _bgcolor The color of the background the font is drawn on (as we can't do color key blitting!)
	 * \param _hilightcolor An alternate color in which the font should appear when hilighted. if not defined, no hilight font will be created
	 * \param _hilightbg An alternate background color for the highlighted font. If undefined, the normal background color will be used
	 * \param style font decoration style (bold, italic, underlined etc)
	 * \param styleset The styleset this font is a part of
	 * \note See orbiter api reference of oapiCreateFont() for more explanation on height, face, proportional and style,
	 *	as these arguments will be passed unaltered to sketchpad to draw the font.
	 */
	static GUI_font *MakeFont(int height, string face, bool proportional, string _id, GUI_COLOR _color, GUI_COLOR _bgcolor,
		GUI_COLOR _hilightcolor = GUI_COLOR(0, 0, 0), GUI_COLOR _hilightbg = GUI_COLOR(0, 0, 0), FontStyle style = FONT_NORMAL, string styleset = "default");

	/**
	 * \brief Creates a new style and adds it to a styleset.
	 *
	 * Set the style properties after creation.
	 * \param styleId The identifier of the style
	 * \param inherit_from The created style will inherit all properties from this style. Pass STYLE_NONE if you don't have a style to inherit from
	 * \param styleset The styleset this style should be added to.
	 */
	static GUI_ElementStyle *CreateStyle(string styleId, string inherit_from = "", string styleset = "default");

	/** 
	 * \brief Creates all the fonts. Currently all of them hardcoded.
	 * \todo replace this with a parser
	 */
	static void createFonts();
	
	/**
	 * \brief Creates all the styles. Currently all hardcoded
	 * \todo Replace this with a parser
	 */
	static void createStyles();
	
	/**
	 * Creates a new StyleSet
	 * \param name The name of the styleset. Must be unique!
	 */
	static void createStyleSet(string name);


};

