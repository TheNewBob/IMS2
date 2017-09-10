#include "GUI_Common.h"




GUI_Draw::GUI_Draw()
{
}


GUI_Draw::~GUI_Draw()
{
}

//returns a texture surface with the proper background color to draw the GUI element on
SURFHANDLE GUI_Draw::createElementBackground(GUI_ElementStyle *style, int width, int height)
{
	SURFHANDLE srf = oapiCreateSurfaceEx(width, height, OAPISURFACE_SKETCHPAD);
	
	//Reminder: DWORD color formatting for oapiColourFill is NOT the same as for Sketchpad operations.
	//don't use getDWORDColor here, and don't use oapiGetColor when working with Sketchpad!
	DWORD color = oapiGetColour(style->_keycolor.r, style->_keycolor.g, style->_keycolor.b);
	oapiColourFill(srf, color, 0, 0, width, height);

	//set colorkey for the surface.
	oapiSetSurfaceColourKey(srf, color);

	return srf;
}

/* fills the designated rectangle on the designated surface with the designated color
*/
void GUI_Draw::ColorFill(RECT rectangle, SURFHANDLE srf, GUI_COLOR color)
{
	//Reminder: DWORD color formatting for oapiColourFill is NOT the same as for Sketchpad operations.
	//don't use getDWORDColor here, and don't use oapiGetColor when working with Sketchpad!
	DWORD orbcolor = oapiGetColour(color.r, color.g, color.b);
	oapiColourFill(srf, orbcolor, rectangle.left, rectangle.top, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top);
}


//draws a rounded rectangle with the passed sketchpad. Works really lousy for larger linewidths
void GUI_Draw::DrawRectangle(RECT rectangle, Sketchpad *skp, GUI_ElementStyle *style, bool highlight)
{
	//calculating the entire linewidth and adjusting drawing position to fit the whole thing in the rect
	int linewidth = style->_linewidth;
	int inset = (int)ceil((float)linewidth / 2.0);
	RECT drawrect = _R(rectangle.left + inset, rectangle.top + inset, rectangle.right - inset, rectangle.bottom - inset);

	//create the vertex list for the pollygon
	int numvertices;
	IVECTOR2 *verts = createRoundedRectVertexList(drawrect, style->_cornerradius, numvertices, style->_roundcorners);

	//prepare the sketchpad
	Pen *pen = oapiCreatePen(1, style->_linewidth, getDwordColor(style->_color));
	skp->SetPen(pen);
	Brush *brush = NULL;
	if (highlight)
	{
		//this element is highlighted, fill with hilightcolor
		brush = oapiCreateBrush(getDwordColor(style->_hilightcolor));
	}
	else if (style->_fillcolor != GUI_COLOR(-1, -1, -1))
	{
		//this element has a fill color, set brush
		brush = oapiCreateBrush(getDwordColor(style->_fillcolor));
	}
	skp->SetBrush(brush);

	//draw the rectangle
	skp->Polygon(verts, numvertices);
	oapiReleasePen(pen);
	if (brush) oapiReleaseBrush(brush);
	delete[] verts;
}

//draws a line
void GUI_Draw::DrawLine(int x0, int y0, int x1, int y1, Sketchpad *skp, GUI_ElementStyle *style)
{
	Pen *pen = oapiCreatePen(1, style->_linewidth, getDwordColor(style->_color));
	skp->SetPen(pen);
	skp->Line(x0, y0, x1, y1);
	oapiReleasePen(pen);
}

//draws an arbitrary polygon. does not delete vertex list
void GUI_Draw::DrawPolygon(IVECTOR2 *vertexlist, int numvertices, Sketchpad *skp, GUI_ElementStyle *style, bool highlight)
{
	//prepare the sketchpad
	Pen *pen = oapiCreatePen(1, style->_linewidth, getDwordColor(style->_color));
	skp->SetPen(pen);
	Brush *brush = NULL;
	if (highlight)
	{
		//this element is highlighted, fill with hilightcolor
		brush = oapiCreateBrush(getDwordColor(style->_hilightcolor));
	}
	else if (style->_fillcolor != GUI_COLOR(-1, -1, -1))
	{
		//this element has a fill color, set brush
		brush = oapiCreateBrush(getDwordColor(style->_fillcolor));
	}
	skp->SetBrush(brush);

	//draw the poly
	skp->Polygon(vertexlist, numvertices);
	oapiReleasePen(pen);
	if (brush) oapiReleaseBrush(brush);
}

//creates a vertex list to draw a rounded rectangle
IVECTOR2 *GUI_Draw::createRoundedRectVertexList(RECT rectangle, int corner_radius, int &OUT_numvertices, DWORD corners)
{
	IVECTOR2 *verts = NULL;
	int numvertices = 0;

	//create vertex list
	for (int i = 0; i < 4; ++i)
	{
		IVECTOR2 center;
		switch (i)
		{
			//upper right corner 
		case 0:
			if ((UPPER_RIGHT & corners) != UPPER_RIGHT || corner_radius == 0)
			{
				//this corner is not rounded, just set a corner vertex
				center.x = rectangle.right;
				center.y = rectangle.top;
				verts = addVertexToVertexList(center, numvertices, verts);
				continue;
			}
			center.x = rectangle.right - corner_radius;
			center.y = rectangle.top + corner_radius;
			verts = createRoundedCornerVertexList(center, corner_radius, UPPER_RIGHT, numvertices, NULL);
			break;
			//upper left corner (angle 90, counter-clockwise progression)
		case 1:
			if ((UPPER_LEFT & corners) != UPPER_LEFT || corner_radius == 0)
			{
				//this corner is not rounded, just set a corner vertex
				center.x = rectangle.left;
				center.y = rectangle.top;
				verts = addVertexToVertexList(center, numvertices, verts);
				continue;
			}
			center.x = rectangle.left + corner_radius;
			center.y = rectangle.top + corner_radius;
			verts = createRoundedCornerVertexList(center, corner_radius, UPPER_LEFT, numvertices, verts);
			break;
			//lower left corner (angle 180, counter-clockwise progression)
		case 2:
			if ((LOWER_LEFT & corners) != LOWER_LEFT || corner_radius == 0)
			{
				//this corner is not rounded, just set a corner vertex
				center.x = rectangle.left;
				center.y = rectangle.bottom;
				verts = addVertexToVertexList(center, numvertices, verts);
				continue;
			}
			center.x = rectangle.left + corner_radius;
			center.y = rectangle.bottom - corner_radius;
			verts = createRoundedCornerVertexList(center, corner_radius, LOWER_LEFT, numvertices, verts);
			break;
			//lower right corner (angle 270, counter-clockwise progression)
		case 3:
			if ((LOWER_RIGHT & corners) != LOWER_RIGHT || corner_radius == 0)
			{
				//this corner is not rounded, just set a corner vertex
				center.x = rectangle.right;
				center.y = rectangle.bottom;
				verts = addVertexToVertexList(center, numvertices, verts);
				continue;
			}
			center.x = rectangle.right - corner_radius;
			center.y = rectangle.bottom - corner_radius;
			verts = createRoundedCornerVertexList(center, corner_radius, LOWER_RIGHT, numvertices, verts);
			break;
		}
	}

	OUT_numvertices = numvertices;
	return verts;
}

/* returns an array of length numvertices, containing points on a rounded corner (I.E. a quarter circle)
 * Note that corners will ALWAYS be drawn counter-clockwise, so choose the starting point accordingly
 * start: the starting point from which to draw the corner
 * corner_radius: the radius of the corner in pixel
 * corner: the designation for the corner
 * OUT_numvertices: returns the number of vertices in the returned array. 
 * If previousvertices != NULL, numvertices is expected to contain the number of vertices in previousvertices when passed
 * previousvertices: A list of vertices these vertices should be appended to. 
 * If not NULL, the function will return previousvertices with the new vertices appended, with OUT_numvertices containing the total amount of vertices
 * previousvertices WILL BE DELETED during execution of this function!!*/
 
IVECTOR2 *GUI_Draw::createRoundedCornerVertexList(IVECTOR2 center, int corner_radius, CORNERS corner, int &OUT_numvertices, IVECTOR2 *previousvertices)
{
	//the number of vertices on a full circle would be half its circumference in pixels (a vertex every other pixel on average)
	int numvertices = (int)((float)corner_radius * 2 * PI) / 8 + 1;
	int vertidx = 0;

	//starting angle, ending angle and angle increment between vertices
	double angle;
	double anglestop;
	double anglestep = 90 / (double)numvertices;
	IVECTOR2 * verts;

	if (previousvertices != NULL)
	{
		//an existing vertex list has been passed, allocate memory appropriately and copy it over
		verts = new IVECTOR2[OUT_numvertices + numvertices + 1];
		for (int i = 0; i < OUT_numvertices; ++i)
		{
			verts[i] = previousvertices[i];
		}
		delete[] previousvertices;
		vertidx = OUT_numvertices;
		numvertices += OUT_numvertices;
	}
	else
	{
		verts = new IVECTOR2[numvertices + 1];
	}


	switch (corner)
	{
	case UPPER_RIGHT:
		angle = 0;
		anglestop = 90;
		break;
	case UPPER_LEFT:
		angle = 90;
		anglestop = 180;
		break;
	case LOWER_LEFT:
		angle = 180;
		anglestop = 270;
		break;
	case LOWER_RIGHT:
		angle = 270;
		anglestop = 360;
		break;
	}

	//generate the vertices for this corner
	while (angle <= anglestop + 0.01)			//needs a bit of tolerance due to double inprecisions
	{
		double radangle = angle * PI / 180;
		verts[vertidx].x = (int)round((double)corner_radius * cos(radangle) + center.x);
		verts[vertidx].y = (int)round((double)corner_radius * -sin(radangle) + center.y);

		angle += anglestep;
		vertidx++;
	}

	OUT_numvertices = numvertices + 1;
	return verts;
}


IVECTOR2 *GUI_Draw::addVertexToVertexList(IVECTOR2 vert, int &IN_OUT_numvertices, IVECTOR2 *vertexlist)
{
	IVECTOR2 *newlist;
	if (vertexlist != NULL)
	{
		if (IN_OUT_numvertices == 0)
		{
			throw invalid_argument("passed empty vertex list!");
		}
		//append to list
		newlist = new IVECTOR2[IN_OUT_numvertices + 1];
		for (int i = 0; i < IN_OUT_numvertices; ++i)
		{
			newlist[i] = vertexlist[i];
		}
		delete[] vertexlist;
		newlist[IN_OUT_numvertices] = vert;
	}
	else
	{
		//no list exists yet, just allocate space for the one element
		newlist = new IVECTOR2;
		newlist->x = vert.x;
		newlist->y = vert.y;
	}
	IN_OUT_numvertices += 1;
	return newlist;
}


//convert rgb value to DWORD
DWORD GUI_Draw::getDwordColor(GUI_COLOR color)
{
	//Warning: Color definitions for sketchpad operations are INVERTED (0xBBGGRR, whereas oapiGetColor returns 0xRRGGBB!)
	return oapiGetColour(color.b, color.g, color.r);
}

//takes an alpha value, a color and a background color and returns the DWORD value of color when it is blended
DWORD GUI_Draw::getAlphaCorrectedColor(float alpha, GUI_COLOR &color, GUI_COLOR &bgcolor)
{
	GUI_COLOR blended;
	blended.r = (int)(alpha * (float)color.r + (1.0 - alpha) * (float)bgcolor.r);
	blended.g = (int)(alpha * (float)color.g + (1.0 - alpha) * (float)bgcolor.g);
	blended.b = (int)(alpha * (float)color.b + (1.0 - alpha) * (float)bgcolor.b);
	return getDwordColor(blended);
}


