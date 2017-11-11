#pragma once

class GuiXmlLoader
{
public:
	GuiXmlLoader();
	~GuiXmlLoader();

	static void LoadStyleSets();
	static string GetProjectFolder();
	static void SetProjectFolder(string projectfolder);
private:
	static void loadStyleSet(string stylesets_root, string folder);
	static string rootfolder;
	static string projectfolder;
	static tinyxml2::XMLDocument *loadXmlFile(string styleset);
	static void loadFonts(tinyxml2::XMLDocument *file, string styleset);
	static void loadFont(tinyxml2::XMLElement *xmlfont, string styleset);
	static void loadStyles(tinyxml2::XMLDocument *file, string styleset);
	static void loadStyle(tinyxml2::XMLElement *xmlstyle, string styleset);
	static void loadStyleAttribute(tinyxml2::XMLElement *xmlattribute, GUI_ElementStyle *style, string styleset);
	static RECT_DOUBLE GuiXmlLoader::readRect(tinyxml2::XMLElement *element);
	static void createDefaultConfig();
	static void loadDefaultConfig();
};

