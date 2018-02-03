#include "GUI_Common.h"
#include "Helpers.h"
#include "StyleSet.h"


StyleSet::StyleSet()
{
}


StyleSet::~StyleSet()
{
	for (auto i = styles.begin(); i != styles.end(); ++i)
	{
		delete i->second;
	}
	for (auto i = fonts.begin(); i != fonts.end(); ++i)
	{
		delete i->second;
	}
}


void StyleSet::AddStyle(GUI_ElementStyle *style, string styleid)
{
	//check if the style already exists
	if (styles.find(styleid) == styles.end())
	{
		styles[styleid] = style;
	}
	else
	{
		Olog::warn("duplicate style definition!");
	}
}

void StyleSet::AddFont(GUI_font *font, string fontid)
{
	//check if the style already exists
	if (fonts.find(fontid) == fonts.end())
	{
		fonts[fontid] = font;
	}
	else
	{
		Olog::warn("duplicate font definition!");
	}

}



GUI_ElementStyle *StyleSet::GetStyle(string styleId)
{
	if (styleId == "")
	{
		styleId = "default";
	}

	map<string, GUI_ElementStyle*>::iterator i = styles.find(styleId);
	if (i == styles.end())
	{
		throw invalid_argument("Style has not been created yet!");
	}
	return i->second;
}



GUI_font *StyleSet::GetFont(string fontId)
{
	map<string, GUI_font*>::iterator i = fonts.find(fontId);
	if (i == fonts.end())
	{
		throw invalid_argument("Font has not been created yet!");
	}
	return i->second;
}


