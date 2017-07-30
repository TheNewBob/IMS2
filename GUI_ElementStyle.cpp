
#include "GUI_Common.h"
#include "GUI_ElementStyle.h"
#include "LayoutElement.h"
#include "GUI_Layout.h"
#include "GUI_Looks.h"

GUI_ElementStyle::GUI_ElementStyle()
{
	_color = GUI_COLOR(255, 255, 255);
	_keycolor = GUI_COLOR(255,105,180);
	_hilightcolor = GUI_COLOR(0, 0, 0);
	_fillcolor = GUI_COLOR(0, 0, 0);
	_font = NULL;
}

GUI_ElementStyle::GUI_ElementStyle(GUI_ElementStyle *inherit_from)
{
	_font = inherit_from->_font;
	_color = inherit_from->_color;
	_hilightcolor = inherit_from->_hilightcolor;
	_keycolor = inherit_from->_keycolor;
	_fillcolor = inherit_from->_fillcolor;
	_linewidth = inherit_from->_linewidth;
	_cornerradius = inherit_from->_cornerradius;
	_roundcorners = inherit_from->_roundcorners;
	_margin_top = inherit_from->_margin_top;
	_margin_bottom = inherit_from->_margin_bottom;
	_margin_left = inherit_from->_margin_left;
	_margin_right = inherit_from->_margin_right;
}

GUI_ElementStyle::~GUI_ElementStyle()
{
}

void GUI_ElementStyle::Set(STYLE_PROPERTIES field, string value)
{

	try
	{
		if (field == color || field == hilightcolor || field == keycolor || field == fillcolor)
		{
			//set colors
			GUI_COLOR tempcolor = GUI_Looks::StringToColor(value);
			switch (field)
			{
			case color: 
				_color = tempcolor;
				break;
			case hilightcolor: 
				_hilightcolor = tempcolor;
				break;
			case keycolor:
				_keycolor = tempcolor;
				break;
			case fillcolor:
				_fillcolor = tempcolor;
				break;
			}
		}
		else if (field == linewidth)
		{
			_linewidth = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble(value.data()))));
		}
		else if (field == roundcorners)
		{
			_roundcorners = 0;
			vector<string> tokens;
			Helpers::Tokenize(value, tokens, ",");
			for (UINT i = 0; i < tokens.size(); ++i)
			{
				if (tokens[i] == "1")
				{
					_roundcorners = _roundcorners | UPPER_RIGHT;
				}
				else if (tokens[i] == "2")
				{
					_roundcorners = _roundcorners | UPPER_LEFT;
				}
				else if (tokens[i] == "3")
				{
					_roundcorners = _roundcorners | LOWER_LEFT;
				}
				else if (tokens[i] == "4")
				{
					_roundcorners = _roundcorners | LOWER_RIGHT;
				}
			}

		}
		else if (field == cornerradius)
		{
			_cornerradius = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble(value.data()))));
		}
		else if (field == margin)
		{
			vector<string> tokens;
			Helpers::Tokenize(value, tokens, ",");
			for (UINT i = 0; i < tokens.size(); ++i)
			{
				switch (i)
				{
				case 0:
					_margin_top = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble(tokens[i].data()))));
					break;
				case 1:
					_margin_left = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble(tokens[i].data()))));
					break;
				case 2:
					_margin_bottom = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble(tokens[i].data()))));
					break;
				case 3:
					_margin_right = (int)max(1, (GUI_Layout::EmToPx(Helpers::stringToDouble((tokens[i].data())))));
					break;
				}
			}
		}
	}
	catch (int e)
	{
		throw invalid_argument("Invalid value for style attribute: " + value);
	}
}

DWORD GUI_ElementStyle::GetCorners()
{
	return _roundcorners;
}

void GUI_ElementStyle::SetFont(GUI_font *font)
{
	_font = font;
}

GUI_font *GUI_ElementStyle::GetFont()
{
	return _font;
}

void GUI_ElementStyle::SetColor(GUI_COLOR color)
{ 
	_color = color; 
}


void GUI_ElementStyle::SetKeyColor(GUI_COLOR color)
{ 
	_keycolor = color; 
}

void GUI_ElementStyle::SetHilightColor(GUI_COLOR color)
{
	_hilightcolor = color; 
}

void GUI_ElementStyle::SetFillColor(GUI_COLOR color)
{ 
	_fillcolor = color; 
}

void GUI_ElementStyle::SetMarginTop(double margin)
{
	_margin_top = GUI_Layout::EmToPx(margin); 
}

void GUI_ElementStyle::SetMarginBottom(double margin)
{
	_margin_bottom = GUI_Layout::EmToPx(margin); 
}

void GUI_ElementStyle::SetMarginLeft(double margin)
{
	_margin_left = GUI_Layout::EmToPx(margin); 
}

void GUI_ElementStyle::SetMarginRight(double margin)
{
	_margin_right = GUI_Layout::EmToPx(margin); 
}

void GUI_ElementStyle::SetCornerRadius(double radius)
{
	_cornerradius = GUI_Layout::EmToPx(radius); 
}
