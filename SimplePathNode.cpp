#include "Common.h"
#include "SimpleTreeNode.h"
#include "SimplePathNode.h"


SimplePathNode::SimplePathNode(int _id) : id(_id)
{
}


SimplePathNode::~SimplePathNode()
{
}


void SimplePathNode::ConnectTo(SimplePathNode *_leads_to)
{
	Olog::assertThat([this, _leads_to]() { return find(leads_to.begin(), leads_to.end(), _leads_to) == leads_to.end(); },
		"You're trying to connect a state node to one it's already connected to!");

	leads_to.push_back(_leads_to);
};


bool SimplePathNode::IsConnectedTo(SimplePathNode *targetnode)
{
	if (find(leads_to.begin(), leads_to.end(), targetnode) == leads_to.end())
	{
		return false;
	}
	return true;
}

