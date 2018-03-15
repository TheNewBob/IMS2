#pragma once

/**
 * A global manager that loads and cashes layouts.
 */
class LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();

	/**
	 * \return A collection of all layouts defined in the passed filename.
	 * \note Most elements will only have two layouts. In the event that an element defines different layouts for different widths,
	 *	you'll have to figure out yourself which one to use.
	 */
	static LAYOUTCOLLECTION *GetLayout(string filename);
	
	/**
	 * \return The height of a layout with the appropriate width in the passed file, or -1 if no layout for this width is defined.
	 */
	static int GetLayoutHeight(string filename, int width);

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

