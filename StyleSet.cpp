#include "GUI_Common.h"
#include "StyleSet.h"


StyleSet::StyleSet()
{
}


StyleSet::~StyleSet()
{
}


void StyleSet::AddStyle(GUI_ElementStyle *style, GUI_STYLE styleid)
{
	//check if the style already exists
	if (styles.find(styleid) == styles.end())
	{
		styles[styleid] = style;
	}
	else
	{
		Helpers::writeToLog(string("multiple style definition!"), L_WARNING);
	}
}

void StyleSet::AddFont(GUI_font *font, GUI_FONT fontid)
{
	//check if the style already exists
	if (fonts.find(fontid) == fonts.end())
	{
		fonts[fontid] = font;
	}
	else
	{
		Helpers::writeToLog(string("multiple font definition!"), L_WARNING);
	}

}



GUI_ElementStyle *StyleSet::GetStyle(GUI_STYLE styleId)
{
	map<GUI_STYLE, GUI_ElementStyle*>::iterator i = styles.find(styleId);
	if (i == styles.end())
	{
		throw invalid_argument("Style has not been created yet!");
	}
	return i->second;
}



GUI_font *StyleSet::GetFont(GUI_FONT fontId)
{
	map<GUI_FONT, GUI_font*>::iterator i = fonts.find(fontId);
	if (i == fonts.end())
	{
		throw invalid_argument("Font has not been created yet!");
	}
	return i->second;
}


