#include "Common.h"
#include "Calc.h"
#include "BoundingBox.h"
#include "SimpleShape.h"
#include "Rotations.h"



SimpleShape::SimpleShape()
{
}


SimpleShape::~SimpleShape()
{
	destroyBoundingBox();
}


SimpleShape SimpleShape::Circle(double radius, UINT segments)
{
	SimpleShape circle;
	//calculate the radial step between segments, in radians
	double radialsteps = 2 * PI / segments;
	circle.vertices.resize(segments);
	double curangle = 0.0;
	for (UINT i = 0; i < segments; ++i)
	{
		circle.vertices[i].x = sin(curangle) * radius;
		circle.vertices[i].y = cos(curangle) * radius;
		Calc::RoundVector(circle.vertices[i], 1e6);
		curangle += radialsteps;
	}
	return circle;
}

SimpleShape SimpleShape::Cylinder(double radius, double height, UINT segments)
{
	//create two circles which will be the top and the bottom of the cylinder
	SimpleShape top = Circle(radius, segments);
	SimpleShape	bottom = SimpleShape(top);

	//move the two circles to the top and bottom of the cylinder and merge them
	VECTOR3 offset = _V(0, 0, height / 2);
	top.Translate(offset);
	bottom.Translate(offset * -1);
	top.Merge(bottom);
	return top;
}


SimpleShape SimpleShape::Rectangle(double width, double height)
{
	SimpleShape rectangle;
	double halfwidth = abs(width / 2);
	double halfheight = abs(height / 2);
	rectangle.vertices.resize(4);
	rectangle.vertices[0] = _V(halfheight * -1, 0, halfwidth * -1);
	rectangle.vertices[1] = _V(halfheight, 0, halfwidth * -1);
	rectangle.vertices[2] = _V(halfheight, 0, halfwidth);
	rectangle.vertices[3] = _V(halfheight * -1, 0, halfwidth);
	return rectangle;
}

SimpleShape SimpleShape::Box(double x, double y, double z)
{
	//create two rectangles that will be the top and bottom of the box
	SimpleShape top = Rectangle(z, x);
	SimpleShape bottom = top;

	//move the top and bottom to the appropriate y position and merge them
	VECTOR3 offset = _V(0, abs(y) / 2, 0);
	top.Translate(offset);
	bottom.Translate(offset * -1);
	top.Merge(bottom);
	return top;
}


SimpleShape SimpleShape::Sphere(double radius, UINT segments)
{
	//create a circle that will form the equator of the sphere
	SimpleShape equator = SimpleShape::Circle(radius, segments);
	//we need to know if the segmentation will lead to poles naturally or if we have to add them.
	bool haspoles = false;
	//the number of times we have to cut a half-sphere is effectively a quarter of its segments.
	//Note that this operation will drop the required segmentation in order of getting equally distributed 
	//vertices considering that it will have pole vertices.
	//for example, a 6-segmentation would not actually end up with pole vertices, and if we just add them
	//we'll end up with an uneven distribution. The following operation will effectively result in a six-segmentation 
	//becoming an 8-segmentation, but only int the vertical of the sphere. Radial segmentation remains unaffected.
	UINT ncuts = segments / 4;
	//basically, if the number of segments on a half-sphere is an even number, the segmentation has a natural pole	
	if ((segments / 2) % 2 == 0)
	{
		haspoles = true;
		//if the segmentation has a natural pole, it means that that cut is merely tangential. We don't need it!
		ncuts -= 1;
	}
	double radialstep = (PI / 2) / (ncuts + 1);

	//now it's time to cut the sphere to slices
	vector<SimpleShape> upperhalfspherecuts;
	vector<SimpleShape> lowerhalfspherecuts;
	upperhalfspherecuts.resize(ncuts);
	lowerhalfspherecuts.resize(ncuts);

	double curangle = radialstep;
	for (UINT i = 0; i < ncuts; ++i)
	{
		//First we need to know the radius of the sphere slice at that latitude
		double curradius = cos(curangle) * radius;
		//an then we need to know how far offset the slice i from the center
		double zoffset = sin(curangle) * radius;
		//now we can create a new circle to represent that slice of the sphere
		upperhalfspherecuts[i] = SimpleShape::Circle(curradius, segments);
		lowerhalfspherecuts[i] = upperhalfspherecuts[i];
		//move the slices of the upper half sphere upwards, of the lower half sphere downwards
		upperhalfspherecuts[i].Translate(_V(0, 0, zoffset));
		lowerhalfspherecuts[i].Translate(_V(0, 0, zoffset * -1));
	}
	//Now we have all the slices, let's put them together
	for (UINT i = 0; i < ncuts; ++i)
	{
		equator.Merge(upperhalfspherecuts[i]);
		equator.Merge(lowerhalfspherecuts[i]);
	}
	//finally, place two vertices for the poles, and we're done
	equator.vertices.push_back(_V(0, 0, radius));
	equator.vertices.push_back(_V(0, 0, radius * -1));
	return equator;
}

void SimpleShape::Merge(SimpleShape merger)
{
	for (UINT i = 0; i < merger.vertices.size(); ++i)
	{
		vertices.push_back(merger.vertices[i]);
	}

	mergeBoundingBox(merger.GetBoundingBox());
}


void SimpleShape::Translate(VECTOR3 offset)
{
	//transformations necessarily invalidate an axis-aligned bounding box.
	destroyBoundingBox();
	//move all vertices by the shapes offset
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i] += offset;
	}

}


void SimpleShape::Rotate(MATRIX3 matrix)
{
	//transformations necessarily invalidate an axis-aligned bounding box.
	destroyBoundingBox();
	//transform all vertices in the shape
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = mul(matrix, vertices[i]);
	}
}


void SimpleShape::Scale(VECTOR3 scale)
{
	//transformations necessarily invalidate an axis-aligned bounding box.
	destroyBoundingBox();
	//scale all vertices in the shape
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i].x *= scale.x;
		vertices[i].y *= scale.y;
		vertices[i].z *= scale.z;
	}
}

BoundingBox *SimpleShape::GetBoundingBox()
{
	//if there's no valid bounding box at the moment, create it
	if (boundingbox == NULL)
	{
		createBoundingBox();
	}
	return boundingbox;
}


void SimpleShape::createBoundingBox()
{
	if (boundingbox == NULL)
	{
		double left = DBL_MAX;
		double right = DBL_MIN;
		double bottom = DBL_MAX;
		double top = DBL_MIN;
		double rear = DBL_MAX;
		double front = DBL_MIN;

		//walk through all vertices and seek the boundaries
		for (auto i = vertices.begin(); i != vertices.end(); ++i)
		{
			if (i->x < left) left = i->x;
			else if (i->x > right) right = i->x;
			if (i->y < bottom) bottom = i->y;
			else if (i->y > top) top = i->y;
			if (i->z < rear) rear = i->z;
			else if (i->z > front) front = i->z;
		}

		//check for 2-dimensional shapes
		if (right == DBL_MIN)
		{
			right = left;
		}
		else if (top == DBL_MIN)
		{
			top = bottom;
		}
		if (front == DBL_MIN)
		{
			front = bottom;
		}

		boundingbox = new BoundingBox(_V(left, bottom, rear), _V(right, top, front));
	}
}

void SimpleShape::destroyBoundingBox()
{
	if (boundingbox != NULL)
	{
		delete boundingbox;
		boundingbox = NULL;
	}
}


bool SimpleShape::mergeBoundingBox(BoundingBox *bb)
{
	//if this currently doesn't have a bounding box, there's no sense in merging them.
	if (boundingbox != NULL)
	{
		boundingbox->MergeBoundingBox(bb);
		return true;
	}
	return false;
}