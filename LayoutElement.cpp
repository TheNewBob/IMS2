#include "LayoutElement.h"


LayoutElement::LayoutElement()
{
}


LayoutElement::~LayoutElement()
{
}


double LayoutElement::GetMarginLeft() 
{
	return margin.left; 
}

void LayoutElement::SetMarginLeft(double margin_left) 
{
	margin.left = margin_left; 
}

double LayoutElement::GetMarginTop() 
{
	return margin.top; 
}

void LayoutElement::SetMarginTop(double margin_top) 
{
	margin.top = margin_top; 
}

double LayoutElement::GetMarginRight() 
{
	return margin.right; 
}

void LayoutElement::SetMarginRight(double margin_right) 
{
	margin.right = margin_right; 
}

double LayoutElement::GetMarginBottom() 
{
	return margin.bottom; 
}

void LayoutElement::SetMarginBottom(double margin_bottom) 
{
	margin.bottom = margin_bottom; 
}


double LayoutElement::GetPaddingLeft() 
{
	return padding.left; 
}

void LayoutElement::SetPaddingLeft(double padding_left) 
{
	padding.left = padding_left; 
}


double LayoutElement::GetPaddingTop() 
{
	return padding.top; 
}

void LayoutElement::SetPaddingTop(double padding_top) 
{
	padding.top = padding_top; 
}

double LayoutElement::GetPaddingRight() 
{
	return padding.right; 
}

void LayoutElement::SetPaddingRight(double padding_right) 
{
	padding.right = padding_right; 
}


double LayoutElement::GetPaddingBottom() 
{
	return padding.bottom; 
}

void LayoutElement::SetPaddingBottom(double padding_bottom) 
{
	padding.bottom = padding_bottom; 
}


void LayoutElement::SetMargin(RECT_DOUBLE margin)
{
	this->margin = margin;
}

void LayoutElement::SetPadding(RECT_DOUBLE padding)
{
	this->padding = padding;
}
