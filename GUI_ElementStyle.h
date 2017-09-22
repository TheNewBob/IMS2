#pragma once

/**
 * \file GUI_ElementStyle.h
 */


/**
 * \brief available style property attributes
 */
typedef enum
{
	unknown,
	color,					//!< border color, string input format: "rrr,ggg,bbb"
	hilightcolor,			//!< alternate fill color if the element is hilighted, string input format: "rrr,ggg,bbb" 
	keycolor,				//!< The background color, which is NOT the fill color. Since we have no color-code blitting, we have to blit whole rectangles, so the background color of an element should match the fill color of its parent. string input format: "rrr,ggg,bbb"
	fillcolor,				//!< The color with which the rectangle of the element is filled. string input format: "rrr,ggg,bbb"
	linewidth,				//!< The width of drawn lines, like biorders, in pixel. string input format: "1"
	cornerradius,			//!< corner radius in pixel if this element is to have rounded corners. string input format: "10"
	roundcorners,			//!< bitmask defining which of the 4 borders are to be rounded. string input format: "1,2,3". corners are 1 through 4 for upper right to lower right, counter-clockwise. default is all!
	margin					//!< An inset used for text drawing and clipping. A child element will only draw inside the margin of the parent. Actually more like padding in CSS, sorry for the awkward naming. string input "2,5,2,5" in pixel in the order top,left,bottom,right
} STYLE_PROPERTIES;


/**
 * \brief Defines the appearance of a GUI_BaseElement or one of its subclasses
 */
class GUI_ElementStyle
{
	friend class GUI_Draw;
public:
	GUI_ElementStyle(string styleid);
	/**
	 * \param inherit_from A parent style that serves as a default configuration for all properties of this style
	 */
	GUI_ElementStyle(string styleid, GUI_ElementStyle *inherit_from);
	~GUI_ElementStyle();
	
	/**
	 * \brief General setter for properties that takes a string containing the value
	 * \param field The property to set
	 * \param value The value for the property as a string. Formatting depends on the property being set
	 * \note All size attributes are read as em.
	 */
	void Set(STYLE_PROPERTIES field, string value);
	
	/**
	 * \brief Sets the font to be used for this style
	 * \param font A pointer to an already created font that will be used to print any text in the element
	 */
	void SetFont(GUI_font *font);
	
	/**
	 * \brief Sets the border color of the style
	 * \param color The desired color
	 * \see STYLE_PROPERTIES
	 */
	void SetColor(GUI_COLOR color);

	/**
 	 * \brief Sets the background color of the style
	 * \param color The desired color
	 * \see STYLE_PROPERTIES
	 */
	void SetKeyColor(GUI_COLOR color);

	/**
	 * \brief Sets the highlight color of the style
	 * \param color The desired color
	 * \see STYLE_PROPERTIES
	 */
	void SetHilightColor(GUI_COLOR color);

	/**
	 * \brief Sets the fill color of the style
	 * \param color The desired color
	 * \see STYLE_PROPERTIES
	 */
	void SetFillColor(GUI_COLOR color);

	/**
	 * \brief Sets the top margin in em
	 * \param margin The margin on which children cannot draw, from the top of the element
	 * \see STYLE_PROPERTIES
	 */
	void SetMarginTop(double margin);

	/**
	* \brief Sets the bottom margin in em
	* \param margin The margin on which children cannot draw, from the bottom of the element
	* \see STYLE_PROPERTIES
	*/
	void SetMarginBottom(double margin);

	/**
	* \brief Sets the left margin in em
	* \param margin The margin on which children cannot draw, from the left edge of the element
	* \see STYLE_PROPERTIES
	*/
	void SetMarginLeft(double margin);

	/**
	* \brief Sets the right margin in em
	* \param margin The margin on which children cannot draw, from the right edge of the element
	* \see STYLE_PROPERTIES
	*/
	void SetMarginRight(double margin);

	/**
	* \brief Sets the corner radius in em
	* \param radius the radius by which designated corners will be rounded
	* \see STYLE_PROPERTIES
	*/
	void SetCornerRadius(double radius);

	/**
	 * \return The font belonging to this style
	 */
	GUI_font *GetFont();
	
	/**
	 * \return A bitmask defining which corners of the element should be rounded
	 * \see CORNERS
	 */
	DWORD GetCorners();
	
	/**
	 * \return the id of this style.
	 */
	string GetId(){ return id; };

	/**
	 * \return The line width of the style, in pixel
	 */
	int LineWidth(){ return _linewidth; };
	
	/**
	 * \return The corner radius of the style, in pixel
	 */
	int CornerRadius(){ return _cornerradius; };
	
	/**
	 * \return The line color of this style
	 */
	GUI_COLOR Color(){ return _color; };
	
	/**
	 * \return The key color of this style
	 * \note Not to be confused with the fill color!
	 * \see STYLE_PROPERTIES
	 */
	GUI_COLOR BackgroundColor(){ return _keycolor; };
	
	/**
	 * \return the hilight color of this style
	 */
	GUI_COLOR HilightColor(){ return _hilightcolor; };
	
	/**
	 * \return The fill color of this style
	 */
	GUI_COLOR FillColor(){ return _fillcolor; };

	/**
	 * \return the child style of this element.
	 * \note This will be NULL in most cases, since only few elements have integrated children.
	 */
	GUI_ElementStyle *GetChildStyle() { return childstyle; };

	/**
	 * \brief Sets a childstyle for this element.
	 * Childstyles are for a statically integrated child of the element, like scrollbars,
	 * not for the dynamically added children of pages.
	 */
	void SetChildStyle(GUI_ElementStyle *style) { childstyle = style; };

	/**
	 * \return the top margin in pixel
	 */
	int MarginTop(){ return _margin_top; };

	/**
	* \return the bottom margin in pixel
	*/
	int MarginBottom(){ return _margin_bottom; };

	/**
	* \return the left margin in pixel
	*/
	int MarginLeft(){ return _margin_left; };
	
	/**
	* \return the right margin in pixel
	*/
	int MarginRight(){ return _margin_right; };

private:
	string id = "";											//!< Identifier for this style. Must be unique within styleset.
	GUI_font *_font = NULL;									//!< Pointer to the font that will be used for text output by an element using this style
	GUI_COLOR _color;										//!< Line drawing color 
	GUI_COLOR _hilightcolor;								//!< The color the element should be filled with when highlighted
	GUI_COLOR _keycolor;									//!< Keycolor of this element (i.e. transparent color)
	GUI_COLOR _fillcolor;									//!< The color the element should be filled with.
	int _linewidth = 1;										//!< Width of drawn lines (usually the borders) in pixel
	int _cornerradius = 0;									//!< Corner radius, in pixel
	DWORD _roundcorners = CORNERS::ALL;						//!< Bitmask to define which corners are rounded
	
	int _margin_top = 0;									//!< Top margin in pixel
	int _margin_bottom = 0;									//!< Bottom margin in pixel
	int _margin_left = 0;									//!< Left margin in pixel
	int _margin_right = 0;									//!< Right margin in pixel
	GUI_ElementStyle *childstyle = NULL;					//!some elements have an integrated child, the style for which is saved here.
};

