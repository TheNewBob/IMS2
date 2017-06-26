#include "Common.h"
#include "LayoutElement.h"
#include "LayoutField.h"
#include "LayoutRow.h"


LayoutRow::LayoutRow()
{
}


LayoutRow::~LayoutRow()
{
}


double LayoutRow::GetHeight() 
{ 
	return height; 
}

void LayoutRow::SetHeight(double height_in_em) 
{ 
	height = height_in_em; 
}

void LayoutRow::AddField(LayoutField newfield)
{
	fields.push_back(newfield); 
}
