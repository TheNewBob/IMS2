#include "GUI_Common.h"
#include "FontsAndStyles.h"

FontsAndStyles *FontsAndStyles::instance = NULL;

FontsAndStyles::FontsAndStyles()
{
	createFonts();
	createStyles();
}


FontsAndStyles::~FontsAndStyles()
{
	for (map<string, StyleSet*>::iterator i = stylesets.begin(); i != stylesets.end(); ++i)
	{
		delete i->second;
	}
}


FontsAndStyles *FontsAndStyles::GetInstance()
{
	if (instance == NULL)
	{
		instance = new FontsAndStyles();
	}
	return instance;
}

void FontsAndStyles::DestroyInstance()
{
	if (instance != NULL)
	{
		delete instance;
	}
}







GUI_font *FontsAndStyles::CreateFont(int height, string face, bool proportional, GUI_FONT id, GUI_COLOR color, GUI_COLOR bgcolor,
	GUI_COLOR hilightcolor, GUI_COLOR hilightbg, FontStyle style, string styleset)
{
	GUI_font *newFont = new GUI_font(height, face, proportional, id, color, bgcolor, hilightcolor, hilightbg, style);

	//check if the style actually exists, create if not
	if (stylesets.find(styleset) == stylesets.end())
	{
		createStyleSet(styleset);
		Helpers::writeToLog(string("font created for non-existing styleset \"" + styleset + "\". Creating styleset implicitly"), L_WARNING);
	}
	
	stylesets[styleset]->AddFont(newFont, id);
	return newFont;
}



GUI_ElementStyle *FontsAndStyles::GetStyle(GUI_STYLE styleId, string styleset)
{
	assert(stylesets.find(styleset) != stylesets.end());	//the styleset has not been created!
	return stylesets[styleset]->GetStyle(styleId);
}



GUI_font *FontsAndStyles::GetFont(GUI_FONT fontId, string styleset)
{
	assert(stylesets.find(styleset) != stylesets.end());	//the styleset has not been created!
	if (fontId == GUI_NO_FONT)
	{
		return NULL;
	}
	return stylesets[styleset]->GetFont(fontId);
}



/* creates a new style inherited from inherit_from
* if no inherit_from is passed, the default style will be inherited*/
GUI_ElementStyle *FontsAndStyles::CreateStyle(GUI_STYLE styleId, GUI_STYLE inherit_from, string styleset)
{
	GUI_ElementStyle *parentstyle = NULL;
	GUI_ElementStyle *newstyle;
	//check if the styleset exists
	map<string, StyleSet*>::iterator i = stylesets.find(styleset);
	
	//the styleset doesn't exist. Create it, but throw a warning
	if (i == stylesets.end())
	{
		createStyleSet(styleset);
		Helpers::writeToLog("style created for non-existing styleset \"" + styleset + "\". Creating styleset implicitly", L_WARNING);
	}
	
	if (inherit_from != STYLE_NONE)
	{
		//retrieve the parent style
		try
		{
			parentstyle = i->second->GetStyle(inherit_from);
		}
		catch (invalid_argument e)
		{
			//this exception should eventually be handled in the parser that reads styles from file
			throw invalid_argument("ERROR: parent style does not exist!");
		}
		newstyle = new GUI_ElementStyle(stylesets[styleset]->GetStyle(inherit_from));
	}
	else
	{
		//create the style without a parent
		newstyle = new GUI_ElementStyle();
	}

	stylesets[styleset]->AddStyle(newstyle, styleId);
	return newstyle;
}


//creates the neccessary fonts. If fonts with different colors, sizes or background colors are needed, create them here
void FontsAndStyles::createFonts()
{
	//registering fonts
	CreateFont(16, "Arial", true, GUI_SMALL_DEFAULT_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24), GUI_COLOR(8, 8, 24), GUI_COLOR(0, 255, 255));
	CreateFont(16, "Arial", true, GUI_SMALL_ERROR_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24), GUI_COLOR(255, 0, 0), GUI_COLOR(8, 8, 24));
	CreateFont(20, "Arial", true, GUI_LARGE_DEFAULT_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24));
}



//creates all the styles. play around here if you want to change the skin.
//multiple skins and skin-switching not implemented yet, but will be fairly trivial
void FontsAndStyles::createStyles()
{
	//create default style. This style should define the color scheme, which should not be changed by later styles.

	GUI_ElementStyle *newstyle = CreateStyle(STYLE_DEFAULT, STYLE_NONE);
	newstyle->Set(color, "0, 255, 255");
	newstyle->Set(hilightcolor, "0, 255, 255");
	newstyle->Set(fillcolor, "8, 8, 24");
	newstyle->Set(keycolor, "255,105,180");
	newstyle->Set(linewidth, "1");
	newstyle->Set(roundcorners, "1,3");
	newstyle->Set(cornerradius, "12");
	newstyle->SetFont(GetFont());

	//style for major windows
	newstyle = CreateStyle(STYLE_WINDOW_BORDER);
	newstyle->Set(linewidth, "3");
	newstyle->Set(cornerradius, "20");

	//style for buttons
	newstyle = CreateStyle(STYLE_BUTTON);
	newstyle->Set(margin, "0,0,0,0");
	//style for buttons that have to stand out
	newstyle = CreateStyle(STYLE_BOLD_BUTTON, STYLE_BUTTON);
	newstyle->Set(linewidth, "2");

	//style for checkboxes
	newstyle = CreateStyle(STYLE_CHECKBOX, STYLE_BUTTON);
	newstyle->Set(margin, "5, 0, 0, 0");
	newstyle->SetCornerRadius(newstyle->GetFont()->GetfHeight() / 2);

	//heading
	newstyle = CreateStyle(STYLE_HEADING, STYLE_BUTTON);
	newstyle->SetFont(GetFont(GUI_LARGE_DEFAULT_FONT));

	//Style for an error alert popup
	newstyle = CreateStyle(STYLE_ERROR_ALERT);
	newstyle->Set(color, "255, 0, 0");

	//style used for debuging - it looks so horrible you can't miss it!
	newstyle = CreateStyle(STYLE_DEBUG, STYLE_NONE);
	newstyle->Set(color, "255, 0, 255");
	newstyle->Set(hilightcolor, "255, 255, 255");
	newstyle->Set(keycolor, "0, 0, 255");
	newstyle->Set(linewidth, "1");
	newstyle->Set(roundcorners, "1,3");
	newstyle->Set(cornerradius, "12");
	newstyle->SetFont(GetFont());

	//styles for status bar
	newstyle = CreateStyle(STYLE_STATUSBAR);
	newstyle->Set(roundcorners, "1,2,3,4");
	newstyle->Set(linewidth, "2");

	//tyle for listboxes
	newstyle = CreateStyle(STYLE_LISTBOX, STYLE_DEFAULT);
	newstyle->Set(margin, "5, 10, 5, 10");

}

void FontsAndStyles::createStyleSet(string name)
{
	//since this will one day depend on modder input, we'll handle it with a log message, not an assert
	if (stylesets.find(name) == stylesets.end())
	{
		stylesets[name] = new StyleSet();
	}
	else
	{
		Helpers::writeToLog("Multiple styles named " + name, L_WARNING);
	}
}