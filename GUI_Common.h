/**
 * \file GUI_Common.h
 * \brief Header file that defines and includes most structures and classes that GUI operations use regularly
 */
#pragma once

#include "orbitersdk.h"
#include "assert.h"
#include <string>
#include <vector>
#include <map>

class GUI_Surface;
class GUI_font;

using namespace oapi;
using namespace std;

/**
 * Identifiers fordefault styles
 */
const string STYLE_DEFAULT = "default";				//!< A default style using the properties most commonly used by GUI elements
const string STYLE_PAGE = "page";					//!< Used for important windows that need a more descernible border
const string STYLE_BUTTON = "button";				//!< Style commonly used for buttons
const string STYLE_HEADING = "heading";				//!< A style used for most text headings
const string STYLE_CHECKBOX = "checkbox";			//!< Style that defines the common apearance of checkboxes
const string STYLE_STATUSBAR = "statusbar";			//!< Style for status bars
const string STYLE_SCROLLBAR = "scrollbar";
const string STYLE_LISTBOX = "listbox";
const string STYLE_ERROR = "error";		//!< Style for error popups


/*typedef enum
{
	//identifiers for styles
	STYLE_NONE,					//!< Use this as parent style if you don't inherit from any style
	STYLE_DEFAULT,				//!< A default style using the properties most commonly used by GUI elements
	STYLE_PAGE,		//!< Used for important windows that need a more descernible border
	STYLE_BUTTON,				//!< Style commonly used for buttons
	STYLE_HEADING,				//!< A style used for most text headings
	STYLE_CHECKBOX,				//!< Style that defines the common apearance of checkboxes
	STYLE_STATUSBAR,				//!< Style for status bars
	STYLE_LISTBOX,
	STYLE_ERROR_ALERT			//!< Style for error popups
} string;*/


/**
 * Identifiers for the available fonts
 */
/*typedef enum
{
	//font identifiers
	GUI_NO_FONT,					//!< placeholder for situations where you want to pass an invalid font id
	GUI_SMALL_DEFAULT_FONT,			//!< default small font with hilighting
	GUI_SMALL_ERROR_FONT,			//!< small font where the hilight represents an error message (non-hilight same as GUI_SMALL_DEFAULT_FONT)
	GUI_LARGE_DEFAULT_FONT			//!< default large font without hilight
} GUI_FONT;*/

enum 
{
	//identifiers for GUI element types 
	// Any element that registers with the GUI manager MUST have a locally unique id 
	// (that is, unique to the gui manager instance). For dynamic elements the id should
	// therefore be created dynamically by the manager.

	//main display elements
	GUI_MAIN_DISPLAY,			
	GUI_MAIN_ROOT_BTN,
	GUI_MAIN_ROOT_MNU,
	GUI_MAIN_ROOT_CONSTMNU_BTN,
	GUI_MAIN_ROOT_DECONSTMNU_BTN,
	GUI_MAIN_ROOT_MODULES_BTN,
	GUI_MAIN_ROOT_CONFIGMNU_BTN,

	//main display construction menu elements
	MD_CONSTRUCTION_MNU,
	MD_CM_DOCKED_VESSELS_LIST,
	MD_CM_INT_BTNS_PAGE,
	MD_CM_INTEGRATE_BTN,
	MD_CM_ROTATE_BTN,
	MD_CM_DETATCH_BTN,
	MD_CM_STACKED_VESSELS_LIST,
	MD_CM_UPDATE_STACK_BTN,
	MD_CM_ASSEMBLE_ALL_BTN,

	//main display deconstruction menu elements
	MD_DECONSTRUCTION_MNU,
	MD_DM_MODULES_LIST,
	MD_DM_SELECTED_MODULES_LIST,
	MD_DM_SORTX_BTN,
	MD_DM_SORTY_BTN,
	MD_DM_SORTZ_BTN,
	MD_DM_SYMMETRY_CHKBX,
	MD_DM_DECONSTRUCT_BTN,
	MD_DM_STAGING_BTN,

	//main display modules menu
	MD_MODULES_MENU,
	MD_MODULES_MODSELECTPG,
	MD_MODULES_MODLIST,
	MD_MODULES_MODSELECTBTN,
	MD_MODULES_CTRLPG,
	MD_MODULES_ROOT_BTN,

	//main display config menu
	MD_CONFIG_MENU,
	MD_CONFIG_RCSASSISTCHKBX,
	MD_CONFIG_SCENEDCHKBX,
	MD_CONFIG_STYLESETSLISTBX,
	
	//this marks the beginning of available UIDs for dynamic assignement
	GUI_UID_DOMAIN
};

/**
 * \brief Internal mouse events 
 *
 * Orbiter is somewhat jumpy in its event handling and often you miss the mousedown event.
 * Querrying the orbiter mousedown event and deriving our own button states solves this issue.
 */
typedef enum 
{
	LBNONE,			//!< The left mouse button isn't pressed
	LBDOWN,			//!< The left mousebutton has been pressed just now
	LBPRESSED,		//!< The left mouse button is being held down
	LBUP			//!< The left mouse button has been let up just now
} GUI_MOUSE_EVENT;		

/**
 * \brief Flags for text justification
 */
typedef enum 
{
	T_RIGHT,			//!< x is the left-hand coordinate, and the text should be drawn from there to the right
	T_LEFT,				//!< x is the right-hand coordinate, and the text should be drawn from there to the left
	T_CENTER			//!< x is the center coordinate, and the text should be drawn evenly towards the right and the left
} GUI_TEXT_POS;				

/**
 * \brief Flags for vertical text alignement
 */
typedef enum 
{
	V_TOP,				//!< y is the top coordinate, and that the text should be drawn below it
	V_BOTTOM,			//!< y is the bottom coordinate, and that the text should be drawn above it(this is NOT the baseline.There is no baseline support, sorry)
	V_CENTER			//!< y is the vertical center of the text, and the top of the text is height / 2 above it.
} GUI_TEXT_VERTICAL_POS;	

/**
 * \brief !DEPRECATED! 
 */
typedef enum 
{
	GUI_BASE, 
	GUI_PAGE
} GUI_ELEMENT_TYPE;

/**
 * \brief Structure used to make operations involving rgb-colors less of a hassle
 */
struct GUI_COLOR
{
	int r;												//!< The red value, >= 0 <= 255
	int g;												//!< The green value, >= 0 <= 255
	int b;												//!< The blue value, >= 0 <= 255
	GUI_COLOR(){};

	/**
	 * \brief Quick constructor to easily define a color
	 * \param red The red value, >= 0 <= 255
	 * \param green The green value, >= 0 <= 255
	 * \param blue The blue value, >= 0 <= 255
	 */
	GUI_COLOR(int red, int green, int blue)
	{
		r = red;
		g = green;
		b = blue;
	};

	/**
	 * \return true if all three color values match.
	 * \param c The color to compare this color to.
	 */
	bool operator==(GUI_COLOR c)
	{
		if (r == c.r && g == c.g && b == c.b)
		{
			return true;
		}
		return false;
	};

	/**
	 * \return true if any of the three color values do not match
	 * \param c The color to compare this color to
	 */
	bool operator !=(GUI_COLOR c)
	{
		if (r == c.r && g == c.g && b == c.b)
		{
			return false;
		}
		return true;
	}
};


/**
 * \brief Bitmask for identifying which corners of a rectangle are to be rounded in a style
 */
typedef enum
{
	UPPER_RIGHT = 1 << 0,											//!< Defines the upper right corner as rounded
	UPPER_LEFT = 1 << 1,											//!< Defines the upper left corner as rounded
	LOWER_LEFT = 1 << 2,											//!< Defines the lower left corner as rounded
	LOWER_RIGHT = 1 << 3,											//!< Defines the lower right corner as rounded
	ALL = UPPER_RIGHT | UPPER_LEFT | LOWER_LEFT | LOWER_RIGHT		//!< Shorthand to define all four corners as rounded
} CORNERS;


//I know it is very atypical to have an include at the end of a header.
//these files are included here because all GUI classes need them, but the included files also needs the stuff defined in this header.
//it might be uncommon, but still seems better than having to put the include in every singel GUI-related cpp file.
#include "GUI_Draw.h"
#include "GUI_font.h"
#include "GUI_ElementStyle.h"
#include "StyleSet.h"
#include "GUI_BaseElement.h"
#include "GUI_BaseElementState.h"
#include "GUI_BaseElementResource.h"
#include "tinyxml2.h"

