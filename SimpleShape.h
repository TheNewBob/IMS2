#pragma once

class BoundingBox;

/**
 * \param A simple 3-dimensional geometric shape, with methods for shape manipulation and merging.
 * \note Shapes produced by the factory functions will have orientation 0 0 1  0 1 0, with the origin of the coordinate system being at the center of the shape.
 */
class SimpleShape
{
public:
	SimpleShape();
	~SimpleShape();

	/**
	 * \return A SimpleShape representing a circle. 
	 * \param radius the radius of the circle
	 * \param segments The number of radial segments on the circle
	 */
	static SimpleShape Circle(double radius, UINT segments);

	/**
	 * \return A SimpleShape representing a cylinder. 
	 * \param radius The radius of the cylinder.
	 * \height The height of the cylinder.
	 * \param segments The number of radial segments to be generated.
	 */
	static SimpleShape Cylinder(double radius, double height, UINT segments);

	/**
	 * \return A SimpleShape representing a rectangle.
	 * \param width The dimension of the rectangle in the z axis
	 * \param hegt The dimension of the rectangle in the x axis
	 * \note width and height will always be treated as positive, even if a negative value is passed!
	 * \note Width of the rectangle is along the z axis, height along x. 
	 */
	static SimpleShape Rectangle(double width, double height);
	
	/**
	 * \return A SimpleShape representing a Box. 
	 * \param x The dimension of the box in the x axis
	 * \param y The dimension of the box in the y axis
	 * \param z The dimension of the box in the z axis
	 * \note The origin (0,0,0) of the box represents its center.
	 */
	static SimpleShape Box(double x, double y, double z);

	/**
	 * \return A SimpleShape representing a sphere.
	 * \param radius The radius of the sphere
	 * \param segments Number of radial segments on the sphere
	 * \note the poles of the sphere are aligned with the z-axis!
	 */
	static SimpleShape Sphere(double radius, UINT segments);

	/**
	 * \return The number of vertices in the shape
	 */
	UINT Size() { return vertices.size(); };
	
	/**
	 * \brief merges another shape into this one.
	 * \note Does not check for vertex overlap!
	 * \param merger The shape to be merged with this.
	 */
	void Merge(SimpleShape merger);

	/**
	 * \brief translates a shape by the demanded offset
	 * \param offset The offset by which the shape is to be translated
	 */
	void Translate(VECTOR3 offset);

	/**
	 * \brief Rotates a shape by a matrix
	 * \param matrix rotation matrix containing the desired rotation
	 */
	void Rotate(MATRIX3 matrix);

	/**
	 * \brief Scales a shape in x y z.
	 * \note 0 0 0 is assumed as the origin of the scaling!
	 * \param scale Defines the scale factors in x y z
	 */
	 void Scale(VECTOR3 scale);

	 /**
	  * \brief sets a reference to the shapes vertex list.
	  * \param OUT_vertexlist vector reference that will be set to the shapes vertex list.
	  */
	 void GetVertices(vector<VECTOR3> &OUT_vertexlist) { OUT_vertexlist = vertices; };

	 /**
	  * \brief returns an axis-aligned bounding box of the shape.
	  * \note bounding boxes are generated when this method is first called to avoid unnecessary calculations,
	  *		since they have to be recalculated every time a shape is transformed. Even after generation bounding boxes
	  *		are not regenerated until this method is called again, so you are guaranteed to get the currently valid bounding box
	  *		with a minimum of overhead. Still, alternating boundingbox querying and transformation will result in siginificant
	  *		cost increas and should be avoided!
	  */		
	 BoundingBox *GetBoundingBox();

private:
	vector<VECTOR3> vertices;
	BoundingBox *boundingbox = NULL;

	/**
	 * \brief Creates the bounding box if the shape does not currently have one
	 * \note To recreate a bounding box, call destroyBoundingBox() first.
	 * \see destroyBoundingBox()
	 */
	void createBoundingBox();
	
	/**
	 * \brief Destroys the bounding box if the shape currently has one.
	 * \see createBoundingBox()
	 */
	void destroyBoundingBox();

	/**
	 * \brief Merges a bounding box with the bounding box of this.
	 * \return true if successful, false if not.
	 * \note In order to successfully merge a bounding box, this shape must already have 
	 *	calculated its bounding box. 
	 */
	bool mergeBoundingBox(BoundingBox *bb);

};

