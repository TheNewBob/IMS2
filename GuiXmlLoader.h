#pragma once

class GuiXmlLoader
{
public:
	GuiXmlLoader();
	~GuiXmlLoader();

	static void LoadStyleSets(string stylesets_root);
	static void LoadStyleSet(string stylesets_root, string folder);

private:
	static tinyxml2::XMLDocument *loadXmlFile(string styleset);
	static void loadFonts(tinyxml2::XMLDocument *file, string styleset);
	static void loadFont(tinyxml2::XMLElement *xmlfont, string styleset);
	static void loadStyles(tinyxml2::XMLDocument *file, string styleset);
	static void loadStyle(tinyxml2::XMLElement *xmlstyle, string styleset);
	static void loadStyleAttribute(tinyxml2::XMLElement *xmlattribute, GUI_ElementStyle *style, string styleset);

};

