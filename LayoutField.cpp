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

void LayoutField::SetNestedLayout(GUI_Layout *layout)
{
	assert(nestedlayout == NULL && "You're not allowed to overwrite a nested layout!");

	nestedlayout = layout;
}

