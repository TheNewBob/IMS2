#include "GUI_Common.h"


// #include "GUI_Looks.h"

map<string, StyleSet*> GUI_Looks::stylesets;
map<int, map<int, vector<GUI_ElementResource*>>> GUI_Looks::resources;
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
	Olog::assertThat([styleset]() { return stylesets.find(styleset) != stylesets.end(); }, "the styleset has not been created!");
	return stylesets[styleset]->GetStyle(styleId);
}



GUI_font *GUI_Looks::GetFont(string fontId, string styleset)
{
	Olog::assertThat([styleset]() { return stylesets.find(styleset) != stylesets.end(); }, "the styleset has not been created!");
	Olog::assertThat([fontId]() { return fontId != ""; }, "Cannot pass an empty font id!");
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


SURFHANDLE GUI_Looks::GetResource(GUI_BaseElement *element)
{
	// this code is ugly as sin. It's a seriously excessive overuse of NULL as a state validator.
	GUI_ElementResource *result = findResourceForElement(element);

	if (result == NULL)
	{
		// no resource for this element exists, create one
		result = element->createResources();
		if (result != NULL)
		{
			resources[element->GetWidth()][element->GetHeight()].push_back(result);
			result->addReference(element);
			return result->GetSurface();
		}
	}
	else
	{
		result->addReference(element);
		return result->GetSurface();
	}
	
	return NULL;
}

void GUI_Looks::ReleaseResource(GUI_BaseElement *element)
{
	GUI_ElementResource *resource = findResourceForElement(element, true);
	Olog::assertThat([resource]() { return resource != NULL; }, "Element tries to release resource, but no resource was found!");
	resource->removeReference(element);
	if (resource->NumReferences() == 0)
	{
		delete resource;
	}
}

GUI_ElementResource *GUI_Looks::findResourceForElement(GUI_BaseElement *element, bool findResourceReferencedByElement)
{
	GUI_ElementResource *result = NULL;

	map<int, map<int, vector<GUI_ElementResource*>>>::iterator widthmapit = resources.find(element->GetWidth());
	if (widthmapit != resources.end())
	{
		map<int, vector<GUI_ElementResource*>> &heightmap = widthmapit->second;
		map<int, vector<GUI_ElementResource*>>::iterator heightmapit = heightmap.find(element->GetHeight());
		if (heightmapit != heightmap.end())
		{
			vector<GUI_ElementResource*> &resourcelist = heightmapit->second;
			for (UINT i = 0; i < resourcelist.size(); ++i)
			{
				Olog::assertThat([resourcelist, i]() { return resourcelist[i] != NULL; }, "Something went horribly wrong!");
				
				if (findResourceReferencedByElement) 
				{
					if (resourcelist[i] != NULL &&
						resourcelist[i]->IsReferencedByElement(element))
					{
						// return the resource if it is referenced by the passed element
						result = resourcelist[i];
						// since this option is only used if the intent is to remove a reference,
						// remove the resource if the element is the only reference to it.
						if (result->NumReferences() == 1)
						{
							resourcelist.erase(resourcelist.begin() + i);
						}
						break;
					}
				}
				else 
				{
					if (resourcelist[i]->IsCompatibleWith(element))
					{
						// return the resource if it is compatible with the passed element
						result = resourcelist[i];
						break;
					}
				}
			}
		}
	}
	return result;
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
	if (stylesets.find(name) == stylesets.end())
	{
		stylesets[name] = new StyleSet();
	}
	else
	{
		Olog::warn("Multiple styles named %s", name);
	}
}

