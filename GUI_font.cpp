#include "GUI_Common.h"

GUI_font::GUI_font(int height, string face, bool proportional, int _id, GUI_COLOR _color, GUI_COLOR _bgcolor, GUI_COLOR _hilightcolor, GUI_COLOR _hilightbg, FontStyle style)
{
	fheight = height;
	id = _id;
	hilightbg = _hilightbg;
	hilightcolor = _hilightcolor;
	bgcolor = _bgcolor;
	color = _color;

	if (hilightbg == GUI_COLOR(0, 0, 0))
	{
		hilightbg = bgcolor;
	}
	if (hilightcolor == GUI_COLOR(0, 0, 0))
	{
		hasHighlightFont = false;
	}
	else
	{
		hasHighlightFont = true;
	}

	Font *font = oapiCreateFont(fheight, proportional, (char*)face.data(), style);
	createFont(font);
}

GUI_font::~GUI_font()
{
	oapiDestroySurface(src);
}



void GUI_font::createFont(Font *font)
{
	
	//create character lookup table
	int curpos = 0;
	//we need to create a dummy surface in order to get a sketchpad in order to get the width of individual characters
	SURFHANDLE dummy = oapiCreateSurfaceEx(10, 10, OAPISURFACE_SKETCHPAD);
	Sketchpad *skp = oapiGetSketchpad(dummy);
	skp->SetFont(font);
	//run through all characters from 32 to 126 and note how wide they are
	for (int i = 0; i < 94; ++i)
	{
		charpos[i] = curpos;
		const char c = i + 32;
		//we need a pixel safety distance between letters, otherwise we get overlap
		curpos += (skp->GetTextWidth(&c, 1) + 2);
	}
	oapiReleaseSketchpad(skp);
	oapiDestroySurface(dummy);
	width = curpos;
	
	//the average width of a symbol is the total width of the texture divided by the number of symbols
	fwidth = width / 94;
	

	//now create the actual surface on which the font is going to be
	int height = fheight;
	//if the font has a highlighted version, we need twice the size, because we have to print it twice
	if (hasHighlightFont) height += fheight;
	
	SURFHANDLE tgt = oapiCreateSurfaceEx(width, height, OAPISURFACE_SKETCHPAD);
	
	//fill the font background color
	oapiColourFill(tgt, oapiGetColour(bgcolor.r, bgcolor.g, bgcolor.b), 0, 0, width, fheight);
	//if we have a hilight version of the font, we need another row with different background color
	if (hasHighlightFont)
	{
		oapiColourFill(tgt, oapiGetColour(hilightbg.r, hilightbg.g, hilightbg.b), 0, fheight, width, fheight);
	}

	//everything's set up, now we only need to write all the characters to the right place
	skp = oapiGetSketchpad(tgt);
	skp->SetFont(font);
	skp->SetTextColor(GUI_Draw::getDwordColor(color));

	for (int i = 0; i < 94; ++i)
	{
		const char c = i + 32;
		skp->Text(charpos[i] + 1, 0, &c, 1);
		if (hasHighlightFont)
		{
			skp->SetTextColor(GUI_Draw::getDwordColor(hilightcolor));
			skp->Text(charpos[i], fheight, &c, 1);
			skp->SetTextColor(GUI_Draw::getDwordColor(color));
		}
	}
	//clean up and assign the newly created surface as source
	oapiReleaseSketchpad(skp);
	oapiReleaseFont(font);
	src = tgt;

}



void GUI_font::Print(SURFHANDLE tgt, std::string &text, int _x, int _y, RECT &elementPos, bool highlight, GUI_TEXT_POS tpos, GUI_TEXT_VERTICAL_POS vpos)
{

	int textwidth = GetTextWidth(text);
	int srcy = 0;

	int startx = _x;					//starting position relative to panel

	if (tpos == T_RIGHT)
	{
		startx = max(_x - textwidth, elementPos.left);
	}
	else if (tpos == T_CENTER)
	{
		startx = max(_x - (textwidth / 2), elementPos.left);
	}

	if (vpos == V_BOTTOM)
	{
		_y -= fheight;
	}
	else if (vpos == V_CENTER)
	{
		_y -= fheight / 2;
	}

	if (highlight && hasHighlightFont)
	{
		srcy += fheight;
	}

	int tgtoffset = 0;
	for (UINT i = 0; i < text.length(); ++i)
	{
		//int srcx = srcX + (text.c_str()[i] - 32) * fwidth;
		int ch = text.c_str()[i] - 32;
		int srcx = charpos[ch];
		int charwidth = width;
		if (ch < 93)
		{
			charwidth = charpos[ch + 1];
		}
		charwidth -= srcx;
		int tgtx = startx + tgtoffset;
		tgtoffset += charwidth - 1;

		if (tgtx < elementPos.left || tgtx + charwidth > elementPos.right)
			//making sure function doesn't print beyond elements boundaries
		{
			break;
		}
		oapiBlt(tgt, src, tgtx, _y, srcx, srcy, charwidth - 1, fheight);
	}
}

//returns the length of the text in pixels
int GUI_font::GetTextWidth(string &text)
{
	int width = 0;
	for (string::iterator it = text.begin(); it != text.end(); ++it)
	{
		int ch = *it - 32;
		int charwidth = width;
		if (ch < 93)
		{
			charwidth = charpos[ch + 1];
		}
		charwidth -= charpos[ch];
		width += charwidth - 1;
	}
	return width;
}


int GUI_font::GetId()
{
	return id;
}



int GUI_font::GetfHeight()
{
	return fheight;
}

