#include "GUI_common.h"
#include "tinyxml2.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"
#include "GUI_Looks.h"
#include "GuiXmlLoader.h"
#include <Windows.h>

namespace XML = tinyxml2;
string GuiXmlLoader::rootfolder = "Config/swingshot/";
string GuiXmlLoader::projectfolder = "";
GuiXmlLoader::GuiXmlLoader()
{
}


GuiXmlLoader::~GuiXmlLoader()
{
}

string GuiXmlLoader::GetProjectFolder()
{
	return rootfolder + projectfolder + "/";
}

void GuiXmlLoader::SetProjectFolder(string projectfolder)
{
	assert(GuiXmlLoader::projectfolder == "" && "swingshot project folder can only be set once!");
	GuiXmlLoader::projectfolder = projectfolder;

	// attempt to load swingshot config from the root folder
	try 
	{
		loadDefaultConfig();
	}
	catch (exception e)
	{
		Helpers::writeToLog(string("No valid swingshot config found, creating a new one."), L_WARNING);
		createDefaultConfig();
	}

}

void GuiXmlLoader::createDefaultConfig()
{
	XML::XMLDocument config;
	XML::XMLElement *pxperem = config.NewElement("pixel_per_em");
	pxperem->SetText("16");
	XML::XMLElement *rootfldr = config.NewElement("projects_root");
	rootfldr->SetText(rootfolder.data());
	config.InsertFirstChild(pxperem);
	config.InsertEndChild(rootfldr);
	config.SaveFile(string(rootfolder + "config.xml").data());
}

void GuiXmlLoader::loadDefaultConfig()
{
	XML::XMLDocument *config = loadXmlFile(rootfolder + "config.xml");
	GUI_Layout::pixel_per_em = Helpers::stringToInt(config->FirstChildElement("pixel_per_em")->GetText());
	rootfolder = config->FirstChildElement("projects_root")->GetText();
}

void GuiXmlLoader::LoadStyleSets()
{
	assert(projectfolder != "" && "swingshot project folder has not been set!");
	string stylesets_root = rootfolder + projectfolder + "/stylesets";
	WIN32_FIND_DATA	searchresult;
	HANDLE hfind = NULL;

	string searchstr = stylesets_root + "/*.*";

	// walk through subfolders and load the stylesets there.
	hfind = FindFirstFile(searchstr.data(), &searchresult);
	if (hfind == INVALID_HANDLE_VALUE)
	{
		Helpers::writeToLog(string("No stylesets found in " + stylesets_root + ", installation possibly corrupted!"), L_ERROR);
		throw runtime_error("No stylesets found!");
	}

	do
	{
		if (searchresult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			string stylesetname = searchresult.cFileName;
			if (stylesetname != "." && stylesetname != "..")
			{
				Helpers::writeToLog(string("Loading styleset " + stylesetname), L_DEBUG);
				loadStyleSet(stylesets_root, stylesetname);
			}
		}
	} while (FindNextFile(hfind, &searchresult));
	FindClose(hfind);
	GUI_Looks::GetFont("default");
}

void GuiXmlLoader::loadStyleSet(string stylesets_root, string styleset)
{
	string fontspath = stylesets_root + "/" + styleset + "/fonts.xml";
	string stylespath = stylesets_root + "/" + styleset + "/styles.xml";
	
	// For a valid stleset, fonts.xml and styles.xml have to be present in the folder.
	XML::XMLDocument *fontfile = loadXmlFile(fontspath);
	XML::XMLDocument *stylefile = loadXmlFile(stylespath);

	loadFonts(fontfile, styleset);
	loadStyles(stylefile, styleset);

}


XML::XMLDocument *GuiXmlLoader::loadXmlFile(string filename)
{
	XML::XMLDocument *doc = new XML::XMLDocument;
	doc->LoadFile(filename.data());
	if (doc->ErrorID() != XML::XMLError::XML_SUCCESS)
	{

		string msg = filename + ": " + doc->ErrorName();
		Helpers::writeToLog(msg, L_ERROR);
		throw runtime_error("Error while loading styles, see log.");
	}
	return doc;
}


void GuiXmlLoader::loadStyles(tinyxml2::XMLDocument *file, string styleset)
{
	XML::XMLElement *currentstyle = file->FirstChildElement("style");

	if (currentstyle == NULL)
	{
		throw new runtime_error("XML Styleset has no styles!");
	}

	//iterate over all styles in the file
	while (currentstyle != NULL)
	{
		loadStyle(currentstyle, styleset);
		currentstyle = currentstyle->NextSiblingElement("style");
	}
}

void GuiXmlLoader::loadStyle(tinyxml2::XMLElement *xmlstyle, string styleset)
{
	// first check the id
	XML::XMLElement *attribute = xmlstyle->FirstChildElement("id");
	if (attribute == NULL)
	{
		throw runtime_error("Style has no id!");
	}
	string currentid = attribute->GetText();

	// next, check if the style inherits another style
	attribute = xmlstyle->FirstChildElement("inherit");
	string inherit_from = "";
	if (attribute != NULL)
	{
		inherit_from = attribute->GetText();
	}

	// create style and set its attributes
	GUI_ElementStyle *style = GUI_Looks::CreateStyle(currentid,inherit_from, styleset);
	attribute = xmlstyle->FirstChildElement();
	
	while (attribute != NULL)
	{
		try {
			if (attribute->Name() != "id") loadStyleAttribute(attribute, style, styleset);
		}
		catch (runtime_error e) {
			string msg = "Error in style " + currentid + ": " + e.what();
			Helpers::writeToLog(msg, L_ERROR);
		}
		attribute = attribute->NextSiblingElement();
	}
}


void GuiXmlLoader::loadStyleAttribute(tinyxml2::XMLElement *xmlattribute, GUI_ElementStyle *style, string styleset)
{
	string attr = xmlattribute->Name();
	STYLE_PROPERTIES prop = unknown;

	//fonts, insets and childstyle require special assignment, the rest can be set by simple text properties.
	if (attr == "font") 
	{
		style->SetFont(GUI_Looks::GetFont(xmlattribute->GetText(), styleset));
	}
	else if (attr == "inset")
	{
		RECT_DOUBLE rect = readRect(xmlattribute);
		style->SetMarginLeft(rect.left);
		style->SetMarginTop(rect.top);
		style->SetMarginRight(rect.right);
		style->SetMarginBottom(rect.bottom);

	}
	else if (attr == "childstyle")
	{
		style->SetChildStyle(GUI_Looks::GetStyle(xmlattribute->GetText(), styleset));
	}
	else
	{
		try {
			// identify the attribute and set it in the style
			if (attr == "line-strength") prop = linewidth;
			else if (attr == "line-color") prop = color;
			else if (attr == "bg-color") prop = fillcolor;
			else if (attr == "key-color") prop = keycolor;
			else if (attr == "hilight-color") prop = hilightcolor;
			else if (attr == "corner-radius") prop = cornerradius;
			else if (attr == "rounded-corners") prop = roundcorners;
			else if (attr == "inset") prop = margin;
			
			if (prop != unknown)
			{
				style->Set(prop, xmlattribute->GetText());
			}
		}
		catch (invalid_argument e) {
			string msg = "Error while parsing tag " + attr + ": " + e.what();
			throw runtime_error(msg);
		}
	}
}


void GuiXmlLoader::loadFonts(tinyxml2::XMLDocument *file, string styleset)
{
	XML::XMLElement *currentfont = file->FirstChildElement("font");

	if (currentfont == NULL)
	{
		throw new runtime_error("XML Styleset has no fonts!");
	}

	//iterate over all styles in the file
	while (currentfont != NULL)
	{
		loadFont(currentfont, styleset);
		currentfont = currentfont->NextSiblingElement("font");
	}
}

void GuiXmlLoader::loadFont(XML::XMLElement *xmlfont, string styleset)
{
	XML::XMLElement *font_id = xmlfont->FirstChildElement("id");
	XML::XMLElement *font_face = xmlfont->FirstChildElement("face");
	XML::XMLElement *font_size = xmlfont->FirstChildElement("size");
	XML::XMLElement *font_color = xmlfont->FirstChildElement("color");
	XML::XMLElement *font_hilight = xmlfont->FirstChildElement("hilight-color");
	XML::XMLElement *font_hilight_bg = xmlfont->FirstChildElement("hilight-background");
	XML::XMLElement *font_key = xmlfont->FirstChildElement("key-color");
	XML::XMLElement *font_emphasis = xmlfont->FirstChildElement("emphasis");

	string id, face;
	int size;
	GUI_COLOR color, hilight, hilight_bg, key;
	FontStyle emphasis;

	if (font_id == NULL) throw runtime_error("Font has no <id>!");
	id = font_id->GetText();

	//lots of boring validation and defaulting
	if (font_face == NULL)
	{
		face = "arial";
		Helpers::writeToLog(string("Font " + id + " has no <face>, using arial"), L_WARNING);
	}
	else
	{
		face = font_face->GetText();
	}

	if (font_size == NULL)
	{
		size = GUI_Layout::EmToPx(1);
		Helpers::writeToLog(string("Font " + id + " has no <size>, using 1em"), L_WARNING);
	}
	else
	{
		size = GUI_Layout::EmToPx(Helpers::stringToDouble(font_size->GetText()));
	}

	if (font_color == NULL)
	{
		color = GUI_Looks::StringToColor("255,255,255");
		Helpers::writeToLog(string("Font " + id + " has no <color>, using white"), L_WARNING);
	}
	else
	{
		color = GUI_Looks::StringToColor(font_color->GetText());
	}

		
	if (font_key == NULL)
	{
		key = GUI_Looks::StringToColor("0,0,0");
		Helpers::writeToLog(string("Font " + id + " has no <key-color>, using black"), L_WARNING);
	}
	else
	{
		key = GUI_Looks::StringToColor(font_key->GetText());
	}

	

	if (font_hilight == NULL)
	{
		hilight = GUI_Looks::StringToColor("0,0,0");
	}
	else
	{
		hilight = GUI_Looks::StringToColor(font_hilight->GetText());
	}

	if (font_hilight_bg == NULL)
	{
		hilight_bg = GUI_Looks::StringToColor("255,255,255");
	}
	else
	{
		if (font_hilight == NULL)
		{
			Helpers::writeToLog(string("Font " + id + " has <hilight-background>, but no <hilight-color>. Attribute will have no effect!"), L_WARNING);
		}
		else {
			hilight = GUI_Looks::StringToColor(font_hilight->GetText());
			hilight_bg = GUI_Looks::StringToColor(font_hilight_bg->GetText());
		}
	}

	if (font_emphasis == NULL)
	{
		emphasis = FONT_NORMAL;
	}
	else
	{
		string emph = font_emphasis->GetText();
		if (emph == "italic")
		{
			emphasis = FONT_ITALIC;
		}
		else if (emph == "bold")
		{
			emphasis = FONT_BOLD;
		}
		else if (emph == "underline")
		{
			emphasis = FONT_UNDERLINE;
		}
		else
		{
			Helpers::writeToLog(string("Font " + id + ": Unknown <emphasis>: " + emph + ", using normal"), L_WARNING);
			emphasis = FONT_NORMAL;
		}
	}

	GUI_Looks::MakeFont(size, face, true, id, color, key, hilight, hilight_bg, emphasis, styleset); 
}


RECT_DOUBLE GuiXmlLoader::readRect(tinyxml2::XMLElement *element)
{
	RECT_DOUBLE rect;

	if (element != NULL)
	{
		try
		{
			//a rect with the passed name is defined, try reading the individual values.
			XML::XMLElement *value = element->FirstChildElement("left");
			if (value != NULL)
			{
				rect.left = Helpers::stringToDouble(value->GetText());
			}

			value = element->FirstChildElement("top");
			if (value != NULL)
			{
				rect.top = Helpers::stringToDouble(value->GetText());
			}

			value = element->FirstChildElement("right");
			if (value != NULL)
			{
				rect.right = Helpers::stringToDouble(value->GetText());
			}

			value = element->FirstChildElement("bottom");
			if (value != NULL)
			{
				rect.bottom = Helpers::stringToDouble(value->GetText());
			}
		}
		catch (exception e)
		{
			string rectname = element->Name();
			string msg = "An error has occured while reading the rect \"" + rectname + "\": " + e.what();
			throw runtime_error(msg.data());
		}
	}


	return rect;
}
