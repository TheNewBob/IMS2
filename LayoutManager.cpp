#include "Common.h"
#include "LayoutElement.h"
#include "LayoutField.h"
#include "LayoutRow.h"
#include "GUI_Layout.h"
#include "tinyxml2.h"
#include "LayoutManager.h"

namespace XML = tinyxml2;

map<string, LAYOUTCOLLECTION*> LayoutManager::layoutcollections;
string LayoutManager::defaultpath = "config/IMS2/GUI/layouts/default";
string LayoutManager::layoutpath = "";

LayoutManager::LayoutManager()
{
}


LayoutManager::~LayoutManager()
{
}


LAYOUTCOLLECTION *LayoutManager::GetLayout(string filename)
{
	auto layoutit = layoutcollections.find(filename);
	LAYOUTCOLLECTION *layouts = NULL;
	if (layoutit == layoutcollections.end())
	{
		XML::XMLDocument *doc;
		//layout is not cashed, try to load from file.
		if (layoutpath == "")
		{
			//no custom layout given, load from default
			doc = loadXmlFile(defaultpath + "/" + filename);
		}
		else
		{
			//there's a layout path given, try loading from there, revert to default if not found.
			try
			{
				doc = loadXmlFile(layoutpath + "/" + filename);
			}
			catch (runtime_error)
			{
				doc = loadXmlFile(defaultpath + "/" + filename);
			}
		}
		
		try
		{
			layouts = loadLayoutCollectionFromXml(doc);
		}
		catch (exception e)
		{
			string msg = "Error while loading Layout " + filename + ": " + e.what();
			Helpers::writeToLog(msg, L_ERROR);
			throw runtime_error("Error while loading layouts!");
		}

		delete doc;
	}
	else
	{
		layouts = layoutit->second;
	}

	return layouts;
}

LAYOUTCOLLECTION *LayoutManager::loadLayoutCollectionFromXml(tinyxml2::XMLDocument *doc)
{
	XML::XMLElement *layout = doc->FirstChildElement("layout");

	if (layout == NULL)
	{
		throw runtime_error("No layouts in XML file!");
	}

	LAYOUTCOLLECTION *layouts = new LAYOUTCOLLECTION;

	while (layout != NULL)
	{
		GUI_Layout *newlayout = loadLayoutFromXml(layout);
		int minwidth = 0;
		//check if the layout has a minimum width declared.
		XML::XMLElement *xmlminwidth = layout->FirstChildElement("minwidth");
		if (xmlminwidth != NULL)
		{
			minwidth = Helpers::stringToInt(xmlminwidth->GetText());
		}
		layouts->AddLayout(minwidth, newlayout);

		layout = layout->NextSiblingElement("layout");
	}

	return layouts;
}


XML::XMLDocument *LayoutManager::loadXmlFile(string filename)
{
	XML::XMLDocument *doc = new XML::XMLDocument;
	doc->LoadFile(filename.data());
	if (doc->ErrorID() != XML::XMLError::XML_SUCCESS)
	{

		string msg = filename + ": " + doc->ErrorName();
		Helpers::writeToLog(msg, L_ERROR);
		throw runtime_error("Error while loading layouts, see log.");
	}

//	string bugme = doc->FirstChild()->GetText

	return doc;
}

GUI_Layout *LayoutManager::loadLayoutFromXml(tinyxml2::XMLElement *layout)
{
	GUI_Layout *newlayout = new GUI_Layout;
	setLayoutElementAttributesFromXml(*newlayout, layout);

	XML::XMLElement *currentrow = layout->FirstChildElement("row");

	if (currentrow == NULL)
	{
		throw new runtime_error("XML layout has no rows!");
	}

	//set the layouts margins if they are defined
	XML::XMLElement *margin = layout->FirstChildElement("margin-left");
	if (margin != NULL)
	{
		newlayout->SetMarginLeft(Helpers::stringToDouble(margin->GetText()));
	}
	margin = layout->FirstChildElement("margin-right");
	if (margin != NULL)
	{
		newlayout->SetMarginRight(Helpers::stringToDouble(margin->GetText()));
	}

	while (currentrow != NULL)
	{
		LayoutRow newrow = loadRowFromXml(currentrow);
		newlayout->AddRow(newrow);
		currentrow = currentrow->NextSiblingElement("row");
	} 

	return  newlayout;
}


LayoutRow LayoutManager::loadRowFromXml(tinyxml2::XMLElement *row)
{
	if (row == NULL)
	{
		throw invalid_argument("NULL row passed to loadRowFromXml()!");
	}

	LayoutRow newrow;
	setLayoutElementAttributesFromXml(newrow, row);

	XML::XMLElement *xmlheight = row->FirstChildElement("height");
	if (xmlheight != NULL)
	{
		newrow.SetHeight(Helpers::stringToDouble(xmlheight->GetText()));
	}

	XML::XMLElement *currentfield = row->FirstChildElement("field");

	if (currentfield == NULL)
	{
		throw runtime_error("XML layout row has no fields!");
	}

	while (currentfield != NULL)
	{
		newrow.AddField(loadFieldFromXml(currentfield));
		currentfield = currentfield->NextSiblingElement("field");
	}
	return newrow;
}


LayoutField LayoutManager::loadFieldFromXml(tinyxml2::XMLElement *field)
{
	if (field == NULL)
	{
		throw invalid_argument("NULL field passed to loadFieldFromXml()!");
	}

	LayoutField newfield;
	setLayoutElementAttributesFromXml(newfield, field);


	XML::XMLElement *fieldwidth = field->FirstChildElement("width");
	if (fieldwidth != NULL)
	{
		newfield.SetWidth(Helpers::stringToDouble(fieldwidth->GetText()));
	}
	else
	{
		throw runtime_error("Layout field in XML is missing width!");
	}

	XML::XMLElement *fieldelement = field->FirstChildElement("element");
	if (fieldelement != NULL)
	{
		newfield.SetElementId(fieldelement->GetText());
	}

	//check if the field has a nested layout
	XML::XMLElement *xmllayout = field->FirstChildElement("layout");
	if (xmllayout != NULL)
	{
		newfield.SetNestedLayout(loadLayoutFromXml(xmllayout));
	}

	return newfield;
}

void LayoutManager::setLayoutElementAttributesFromXml(LayoutElement &element, tinyxml2::XMLElement *xmlelement)
{
	element.SetMargin(getRectFromXml(xmlelement, "margin"));
	element.SetPadding(getRectFromXml(xmlelement, "padding"));
}

RECT_DOUBLE LayoutManager::getRectFromXml(tinyxml2::XMLElement *element, string rect_id)
{
	RECT_DOUBLE rect;
	XML::XMLElement *xmlrect = element->FirstChildElement(rect_id.data());

	if (xmlrect != NULL)
	{
		try
		{
			//a rect with the passed name is defined, try reading the individual values.
			XML::XMLElement *value = xmlrect->FirstChildElement("left");
			if (value != NULL)
			{
				rect.left = Helpers::stringToDouble(value->GetText());
			}

			value = xmlrect->FirstChildElement("top");
			if (value != NULL)
			{
				rect.top = Helpers::stringToDouble(value->GetText());
			}

			value = xmlrect->FirstChildElement("right");
			if (value != NULL)
			{
				rect.right = Helpers::stringToDouble(value->GetText());
			}

			value = xmlrect->FirstChildElement("bottom");
			if (value != NULL)
			{
				rect.bottom = Helpers::stringToDouble(value->GetText());
			}
		}
		catch (exception e)
		{
			string msg = "An error has occured while reading the rect \"" + rect_id + "\": " + e.what();
			throw runtime_error(msg.data());
		}
	}


	return rect;
}