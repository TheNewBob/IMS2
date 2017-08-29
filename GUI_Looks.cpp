#include "GUI_Common.h"
#include "GUI_Looks.h"

map<string, StyleSet*> GUI_Looks::stylesets;
string GUI_Looks::defaultStyle = "default";


GUI_font *GUI_Looks::MakeFont(int height, string face, bool proportional, string id, GUI_COLOR color, GUI_COLOR bgcolor,
	GUI_COLOR hilightcolor, GUI_COLOR hilightbg, FontStyle style, string styleset)
{
	GUI_font *newFont = new GUI_font(height, face, proportional, id, color, bgcolor, hilightcolor, hilightbg, style);

	//check if the style actually exists, create if not
	if (stylesets.find(styleset) == stylesets.end())
	{
		createStyleSet(styleset);
	}
	
	stylesets[styleset]->AddFont(newFont, id);
	return newFont;
}



GUI_ElementStyle *GUI_Looks::GetStyle(string styleId, string styleset)
{
	assert(stylesets.find(styleset) != stylesets.end());	//the styleset has not been created!
	return stylesets[styleset]->GetStyle(styleId);
}



GUI_font *GUI_Looks::GetFont(string fontId, string styleset)
{
	assert(stylesets.find(styleset) != stylesets.end());	//the styleset has not been created!
	assert(fontId != "" && "Cannot pass an empty font id!");
	return stylesets[styleset]->GetFont(fontId);
}

GUI_COLOR GUI_Looks::StringToColor(string strRGB)
{
	vector<string> tokens;
	Helpers::Tokenize(strRGB, tokens, ",");
	if (tokens.size() != 3) throw runtime_error("Color string must contain 3 comma-separated values between 0 and 255 (like 255,255,255)");
	int blue, green, red;
	red = atoi(tokens[0].data());
	green = atoi(tokens[1].data());
	blue = atoi(tokens[2].data());
	GUI_COLOR color;
	color.r = red;
	color.g = green;
	color.b = blue;
	return color;
}

vector<string> GUI_Looks::GetAvailableStyleSets()
{
	vector<string> retval;
	for (auto i = stylesets.begin(); i != stylesets.end(); ++i)
	{
		retval.push_back(i->first);
	}
	return retval;
}



GUI_ElementStyle *GUI_Looks::CreateStyle(string styleId, string inherit_from, string styleset)
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
	
	if (inherit_from != "")
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
		newstyle = new GUI_ElementStyle(styleId, stylesets[styleset]->GetStyle(inherit_from));
	}
	else
	{
		//create the style without a parent
		newstyle = new GUI_ElementStyle(styleId);
	}

	stylesets[styleset]->AddStyle(newstyle, styleId);
	return newstyle;
}


void GUI_Looks::createFonts()
{
	//STUB
}

/*void GUI_Looks::createFonts()
{
	//registering fonts
	CreateFont(16, "Arial", true, GUI_SMALL_DEFAULT_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24), GUI_COLOR(8, 8, 24), GUI_COLOR(0, 255, 255));
	CreateFont(16, "Arial", true, GUI_SMALL_ERROR_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24), GUI_COLOR(255, 0, 0), GUI_COLOR(8, 8, 24));
	CreateFont(20, "Arial", true, GUI_LARGE_DEFAULT_FONT, GUI_COLOR(0, 255, 255), GUI_COLOR(8, 8, 24));
}*/

void GUI_Looks::createStyles()
{
	//STUB
}

/*
void GUI_Looks::createStyles()
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

}*/

void GUI_Looks::createStyleSet(string name)
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

