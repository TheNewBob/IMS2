#include "Common.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS_Module.h"
#include "EventTypes.h"
#include "EventHandler.h"
#include "IMS.h"

//this file contains Compare functions to sort modules in various ways

//sort ascending by position in vessel on axis:
bool IMS_Module::X_ORDER(IMS_Module *a, IMS_Module *b)
{
	return a->pos.x < b->pos.x;
}

bool IMS_Module::Y_ORDER(IMS_Module *a, IMS_Module *b)
{
	return a->pos.y < b->pos.y;
}

bool IMS_Module::Z_ORDER(IMS_Module *a, IMS_Module *b)
{
	return a->pos.z < b->pos.z;
}

//sort ascending by absolute position in vessel on axis, resulting in a symmetric sorting where
//modules at the same position on each side will be immediately after each other.
//the order is ascending, I.E. modules closer to zero will be listed first
bool IMS_Module::X_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b)
{
	return abs(a->pos.x) < abs(b->pos.x);
}

bool IMS_Module::Y_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b)
{
	return abs(a->pos.y) < abs(b->pos.y);
}

bool IMS_Module::Z_SYMMETRIC_ORDER(IMS_Module *a, IMS_Module *b)
{
	return abs(a->pos.z) < abs(b->pos.z);
}


//sorts attachment points by position. Used for finding overlapping points
bool IMS_Module::ATTACHMENT_POINTS_BY_POS(IMSATTACHMENTPOINT *a, IMSATTACHMENTPOINT *b)
{
	//we introduce a weighting to the axes to avoid symmetrical duplicates in the ordering.
	//The goal of this comparison is that we get overlapping points neighbouring each other, not to get an idea of where they are.
	double pos_a = a->pos.x + a->pos.y * 10 + a->pos.z * 100;
	double pos_b = b->pos.x + b->pos.y * 10 + b->pos.z * 100;

	return pos_a < pos_b;
}
