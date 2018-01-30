#pragma once

/**
 * \brief Defines a simple axis-aligned bounding box for a SimpleShape.
 *
 * Think of this as a nested class of SimpleShape, really.
 */
class BoundingBox
{
public:
	friend class SimpleShape;
	/**
 	 * \param left_lower_rear The left-lower-rear corner of the bounding box, i.e. the minimum values in x, y and z
	 * \param right_top_front The right-top-front corner of the bounding box, i.e. the maximum values in x, y and z
	 */
	BoundingBox(VECTOR3 left_lower_rear, VECTOR3 right_top_front)
	{
		initBoundingBox(left_lower_rear, right_top_front);
	};

	/**
	 * \brief Creates a new boudning box identical to the passed pointer
	 */
	BoundingBox(BoundingBox *bb)
	{
		initBoundingBox(_V(bb->left, bb->bottom, bb->rear),
						_V(bb->right, bb->top, bb->front));
	}

	~BoundingBox() {};

	/**
	 * \return The left-hand limit of the bounding box, or minimum x-value
	 */
	double GetLeft() { return left; };

	/**
	* \return The right-hand limit of the bounding box, or maximum x-value
	*/
	double GetRight() { return right; };

	/**
	* \return The bottom limit of the bounding box, or minimum y-value
	*/
	double GetBottom() { return bottom; };

	/**
	* \return The top limit of the bounding box, or maximum y-value
	*/
	double GetTop() { return top; };

	/**
	* \return The rear limit of the bounding box, or minimum z-value
	*/
	double GetRear() { return rear; };

	/**
	* \return The front limit of the bounding box, or maximum z-value
	*/
	double GetFront() { return front; };

	/**
	 * \return True if the bounding box is only two-dimensional
	 */
	bool IsTwoDimensional() { return twodee; };
	
	/**
	 * \brief Merges this bounding box with the one passed to it.
	 * The result will be the minimum box that can contain both boxes.
	 * \note It is completely irrelevant to this operation if the boxes are spaced apart.
	 *	The resulting box will encompass both boxes and all the space in between, so be sensible!
	 */
	void MergeBoundingBox(BoundingBox *bb)
	{
		if (bb->left < left)
		{
			left = bb->left;
		}
		if (bb->right > right)
		{
			right = bb->right;
		}
		if (bb->bottom < bottom)
		{
			bottom = bb->bottom;
		}
		if (bb->top > top)
		{
			top = bb->top;
		}
		if (bb->rear < rear)
		{
			rear = bb->rear;
		}
		if (bb->front > front)
		{
			front = bb->front;
		}
	};

private:
	double left;
	double right;
	double bottom;
	double top;
	double front;
	double rear;
	bool twodee = false;

	void initBoundingBox(VECTOR3 &left_lower_rear, VECTOR3 &right_top_front)
	{
		left = left_lower_rear.x;
		bottom = left_lower_rear.y;
		rear = left_lower_rear.z;
		right = right_top_front.x;
		top = right_top_front.y;
		front = right_top_front.z;

		Helpers::assertThat([this]() { return left <= right; }, "Left side of bounding box must be smaller than right side!");
		Helpers::assertThat([this]() { return bottom <= top; }, "Bottom side of bounding box must be smaller than top side!");
		Helpers::assertThat([this]() { return rear <= front; }, "Rear side of bounding box must be smaller than Front side!");

		//check if the box is 2-dimensional
		if (Calc::IsEqual(left, right) ||
			Calc::IsEqual(top, bottom) ||
			Calc::IsEqual(rear, front))
		{
			twodee = true;
		}
	};
};

