#include "Common.h"
#include <queue>
#include <deque>
#include <stack>
#include "SimpleTreeNode.h"
#include "SimplePathNode.h"
#include "PathFinding.h"


PathFinding::PathFinding()
{
}


PathFinding::~PathFinding()
{
}



/* algorithm description:
 * A breadth-first search in concept (i.e. nodes of the same level are all checked before moving on to a deeper level),
 * but with two twists: The common breadth-first search is a tree search, while this deals with a map that is expected
 * to have circular interconnections. The common breadth-first search also does not have path-awareness, since this is 
 * given by the tree it is searching through. You find the node in the tree, you can trace it back to the root. And 
 * again, we don't have a tree.
 * The algorithm solves this by dynamically building a SimpleTree to document its search through the map. Since the
 * job of the SimpleTree structure is to build a stable tree, it is not an efficient structure to search through, which
 * is annoying since because of the circularity of the map we also have to check which nodes have already been looked at.
 * For this reason, the nodes of the SimpleTree are stored in a search tree (an STL map), which might be a bit confusing
 * at first, but this way we get an optimised search structure and a stable representation.
 */
bool PathFinding::BreadthFirst(SimplePathNode *origin, SimplePathNode *target, stack<int> &OUT_path)
{
	//make shure the path is clear... er... empty.
	OUT_path.empty();

	//search tree that will contain the nodes of our SimpleTree,
	//which are also the nodes that were already checked.
	//the SimpleTree itself is a logical construct resulting from
	//the connection of its nodes
	map<int, SimpleTreeNode*> checkednodes;

	//the queue that will handle the breadth-first search
	queue<SimplePathNode*> nodestocheck;
	nodestocheck.push(origin);

	//create the root of the SimpleTree and add it to the searchtree
	SimpleTreeNode *originnode = new SimpleTreeNode(origin->GetId(), NULL);
	checkednodes[origin->GetId()] = originnode;

	//do the actual search
	SimpleTreeNode *targettreenode = NULL;					//this will contain the top node of the SimpleTree (the target) when the search has finished
	while (nodestocheck.size() > 0 && targettreenode == NULL)
	{
		//remove the oldest element from our queue and walk through its neighbors
		SimplePathNode *checknode = nodestocheck.front();
		nodestocheck.pop();
		vector<SimplePathNode*> neighbors = checknode->GetConnected();
		

		//retrieve the treenode of the currently processed PathNode
		SimpleTreeNode *currentparent = checkednodes[checknode->GetId()];
		//walk through neighbors
		for (UINT i = 0; i < neighbors.size() && targettreenode == 0; ++i)
		{
			//check if we already checked this particular node
			if (checkednodes.find(neighbors[i]->GetId()) == checkednodes.end())
			{
				//add the node to the queue so it will eventually get its neighbors checked
				nodestocheck.push(neighbors[i]);
				//insert the node into the SimpleTree
				SimpleTreeNode *neighbornode = new SimpleTreeNode(neighbors[i]->GetId(), currentparent);
				//add the node to the list of already checked elements
				checkednodes[neighbors[i]->GetId()] = neighbornode;
			}
			//if this particular neighbor is the actual target, terminate the search
			//this stands outside the check so the algorithm works when current state and target state
			//are identical. It means that the algorithm will search for the shortest path to loop around
			//and arrive at the origin again. This is necessary for some operations.
			if (neighbors[i] == target)
			{
				//check if the found target node is actually the same as the origin node.
				//this is expected in cases where we sought the shortest path from a node back 
				//to itself through its neighboring nodes
				if (neighbors[i]->GetId() == originnode->GetId())
				{
					//The node will technically be twice in the simple tree after this,
					//but that is not important. The SimpleTree describes a PATH, and every
					//step on that path must be present and connected.
					targettreenode = new SimpleTreeNode(originnode->GetId(), currentparent);
				}
				else
				{
					//the target node will already have been created at line 73
					targettreenode = checkednodes[target->GetId()];
				}
			}
		}
	}

	if (targettreenode == NULL)
	{
		//no path has been found!
		return false;
	}

	//What we end up with is the SimpleTree containing all nodes the search went over,
	//ordered in exactly the way the search found them. All that's left to do is take the
	//SimpleTreeNode for the target pathnode, move up the tree to the root and add the proper
	//pathnode ids to the path.
	SimpleTreeNode *currentnode = targettreenode;

	while (currentnode != NULL)
	{
		OUT_path.push(currentnode->GetId());
		currentnode = currentnode->GetParent();
	}

	//clean up the SimpleTree
	SimpleTreeNode *rootnode = checkednodes[origin->GetId()];
	delete rootnode;

	return true;
}
