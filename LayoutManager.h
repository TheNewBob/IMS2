#pragma once

/**
 * A global manager that loads and cashes layouts.
 */
class LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();

	static LAYOUTCOLLECTION *GetLayout(string filename);

private:

	static map<string, LAYOUTCOLLECTION*> layoutcollections;
	static string layoutpath;
	static tinyxml2::XMLDocument *loadXmlFile(string filename);
	static LAYOUTCOLLECTION *loadLayoutCollectionFromXml(tinyxml2::XMLDocument *doc);
	static GUI_Layout *loadLayoutFromXml(tinyxml2::XMLElement *layout);
	static LayoutRow loadRowFromXml(tinyxml2::XMLElement *row);
	static LayoutField loadFieldFromXml(tinyxml2::XMLElement *field);
	static void setLayoutElementAttributesFromXml(LayoutElement &element, tinyxml2::XMLElement *xmlelement);
	static RECT_DOUBLE getRectFromXml(tinyxml2::XMLElement *element, string rect_id);
};

