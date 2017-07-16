
#include "GUI_Common.h"
#include "GUI_ElementStyle.h"


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
			int blue, green, red;
			vector<string> tokens;
			Helpers::Tokenize(value, tokens, ",");
			if (tokens.size() != 3) throw STYLEPROPERTY_VALUE_INVALID;
			red = atoi(tokens[0].data());
			green = atoi(tokens[1].data());
			blue = atoi(tokens[2].data());
			switch (field)
			{
			case color: 
				_color.r = red;
				_color.g = green;
				_color.b = blue;
				break;
			case hilightcolor: 
				_hilightcolor.r = red;
				_hilightcolor.g = green;
				_hilightcolor.b = blue;
				break;
			case keycolor:
				_keycolor.r = red;
				_keycolor.g = green;
				_keycolor.b = blue;
				break;
			case fillcolor:
				_fillcolor.r = red;
				_fillcolor.g = green;
				_fillcolor.b = blue;
				break;
			}
		}
		else if (field == linewidth)
		{
			_linewidth = atoi(value.data());
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
			_cornerradius = atoi(value.data());
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
					_margin_top = atoi(tokens[i].data());
					break;
				case 1:
					_margin_left = atoi(tokens[i].data());
					break;
				case 2:
					_margin_bottom = atoi(tokens[i].data());
					break;
				case 3:
					_margin_right = atoi(tokens[i].data());
					break;
				}
			}
		}
	}
	catch (int e)
	{
		throw STYLEPROPERTY_VALUE_INVALID;
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

