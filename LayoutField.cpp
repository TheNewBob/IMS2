#include "Common.h"
#include "LayoutElement.h"
#include "LayoutField.h"


LayoutField::LayoutField()
{
}


LayoutField::~LayoutField()
{

}


double LayoutField::GetWidth() 
{
	return width; 
}

void LayoutField::SetWidth(double width) 
{
	this->width = width; 
}

string LayoutField::GetElementId() 
{
	return elementid; 
}

void LayoutField::SetElementId(string element_id) 
{
	elementid = element_id; 
}

string LayoutField::GetElementStyle()
{
	return styleid;
}

void LayoutField::SetElementStyle(string style)
{
	styleid = style;
}

void LayoutField::SetNestedLayout(GUI_Layout *layout)
{
	Helpers::assertThat([this]() { return nestedlayout == NULL; }, "You're not allowed to overwrite a nested layout!");

	nestedlayout = layout;
}

