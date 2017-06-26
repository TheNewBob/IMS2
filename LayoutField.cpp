#include "Common.h"
#include "LayoutElement.h"
#include "LayoutField.h"


LayoutField::LayoutField()
{
}


LayoutField::~LayoutField()
{
}


double LayoutField::GetWidth() { return width; };
void LayoutField::SetWidth(double width) { this->width = width; };
string LayoutField::GetElementId() { return elementid; };
void LayoutField::SetElementId(string element_id) { elementid = element_id; };
