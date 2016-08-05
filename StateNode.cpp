#include "Common.h"
#include "SimplePathNode.h"
#include "StateNode.h"


StateNode::StateNode(int _id, bool _stable, string _description) : SimplePathNode(_id), stable(_stable)
{
	description = _description;
}


StateNode::~StateNode()
{
}
