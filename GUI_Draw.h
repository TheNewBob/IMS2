#pragma once


class GUI_ElementStyle;

/**
* \brief This class contains drawing functions for the GUI
*
* Note that these drawing functions are relatively slow:
* they are intended to be used to draw GUI elements once
* into video memory, while drawing the actual visuals
* to the panel is done by blitting these pre-drawn textures
*
* also note that this is still very much a mock implementation
* it just uses the default Sketchpad commands to draw shapes,
* which is unsatisfactory because the Sketchpad algorithms' suck
* increases proportional with line width. A future implementation
* should aim at a complete and well-working drawing implementation
* on a per-pixel basis using Sketchpad's Pixel()
*/
class GUI_Draw
{
public:
	GUI_Draw();
	~GUI_Draw();

	/**
	 * \brief Allocates a drawing surface and fills it with a background color
	 * \param style The style defining the background color of the element
	 * \param width The width of the surface in pixel
	 * \param height the height of the surface in pixel
	 * \return a Handle to the newly created surface
	 */
	 static SURFHANDLE createElementBackground(GUI_ElementStyle *style, int width, int height);
	
	/**
	* \brief Draws a filled rectangle in the passed style
	*
	* Allows corner radiuses. If you don't want a border, set border-width of the style to 0.
	* \param rectangle Position and Dimensions of the rectangle
	* \param style Pointer to the style the background should be drawn in
	* \param skp Pointer to the sketchpad doing the drawing
	* \param style The style defining the visuals of the rectangle
	* \param hilight If true, the rectangle will be filled with the styles hilight color, otherwise with its fillcolor
	*/
	static void DrawRectangle(RECT rectangle, Sketchpad *skp, GUI_ElementStyle *style, bool hilight = false);

	/**
	 * \brief Draws a line
	 * \param x0 The x-coordinate (surface relative) of the starting point
	 * \param y0 The y-coordinate (surface relative) of the starting point
	 * \param x1 The x-coordinate (surface relative) of the end point
	 * \param y1 The y-coordinate (surface relative) of the end point
	 * \param skp The sketchpad to draw the line with
	 * \param style The style determining linewidth and color
	 */
	static void DrawLine(int x0, int y0, int x1, int y1, Sketchpad *skp, GUI_ElementStyle *style);
	
	/**
	 * \brief Draws an arbitrary filled polygon
	 * \param vertexlist Pointer to an array of vertices. The caller is responsible for freeing the list after use!
	 * \param numvertices The number of vertices in vertexlist
	 * \param skp The sketchpad to draw with
	 * \param style The style determining border color and width as well as fill color. Border radius is ignored.
	 * \param highlight If true, the styles hilightcolor will be used as fill color, otherwise the styles fill color
	 */
	static void DrawPolygon(IVECTOR2 *vertexlist, int numvertices, Sketchpad *skp, GUI_ElementStyle *style, bool highlight = false);
	
	/**
	 * \brief Fills a a rectangle with the designated color
	 * \param rectangle The rectangle to fill, relative to srf
	 * \param srf The drawing surface on which to perform the color fill
	 * \param color The color to fill rectangle with
	 */
	static void ColorFill(RECT rectangle, SURFHANDLE srf, GUI_COLOR color);

	/**
	 * \brief converts a GUI_COLOR to a DWORD formatted for sketchpad operations
	 * \return A DWORD with formatting 0xBBGGRR
	 * \param color The color to convert
	 * \note The DWORD returned by oapiGetColor has the formatting 0xRRGGBB and cannot be used
	 * for sketchpad operations. On the other hand, the formatting returned by this function will not
	 * work for oapiColorFill().
	 */
	static DWORD getDwordColor(GUI_COLOR color);

	/**
	 * \brief takes an alpha value, a color and a background color and returns the DWORD value of color when it is blended
	 * \param alpha The alpha value for which to calculate (transparency if you will)
	 * \param color the overlying color
	 * \param bgcolor The background color
	 * \return the color that results from the alpha-blending of color and bgcolor
	 * \note functional, but currently unused. Was intended for line effects, but because sketchpad gets really
	 * bad at thicker lines that experiment was abandoned until somebody writes better line drawing algorithms.
	 */
	static DWORD getAlphaCorrectedColor(float alpha, GUI_COLOR &color, GUI_COLOR &bgcolor);

private:
	/**
	 * \brief creates a vertex list to draw a rounded rectangle
	 * \param rectangle The position and dimensions of the rectangle
	 * \param corner_radius In pixel
	 * \param OUT_numvertices contains the number of vertices in the list after the operation finishes
	 * \param corners Bitmask describing which corners to round
	 */
	static IVECTOR2 *createRoundedRectVertexList(RECT rectangle, int corner_radius, int &OUT_numvertices, DWORD corners);

	/**
	 * \brief creates a list of points on a rounded corner (I.E. a quarter circle).
	 *
	 * \note Corners will ALWAYS be drawn counter-clockwise, so choose the starting point accordingly
	 * \param center The position of the center of the corner radius
	 * \param corner_radius the radius of the corner in pixel
	 * \param corner The designation for the corner 
	 * \param OUT_numvertices Contains the number of vertices in the returned array after the function finishes.
	 * \note If previousvertices != NULL, OUT_numvertices is expected to already contain the number of vertices in previousvertices when passed
	 * \param previousvertices A list of vertices these vertices should be appended to.
	 *	If not NULL, the function will return previousvertices with the new vertices appended, with OUT_numvertices containing the total amount of vertices
	 * \note previousvertices WILL BE DESTROYED during execution of this function!!
	 */
	static IVECTOR2 *createRoundedCornerVertexList(IVECTOR2 center, int corner_radius, CORNERS corner, int &OUT_numvertices, IVECTOR2 *previousvertices = NULL);
	
	/**
	 * \brief Appends a vertex to a vertex list.
	 * \param vert The new vertex to be added to the list
	 * \param IN_OUT_numvertices returns number of vertices in the list. Expects length of vertexlist as input
	 * \param vertexlist The vertex list vert should be appended to. Will be initialised if NULL. 
	 * \note vertexlist WILL BE DESTROYED during execution of this function!
	 */
	static IVECTOR2 *addVertexToVertexList(IVECTOR2 vert, int &IN_OUT_numvertices, IVECTOR2 *vertexlist);
	
};

