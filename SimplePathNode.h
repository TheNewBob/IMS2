#pragma once

/**
 * \brief The simplest possible kind of pathing node 
 * 
 * It has only neighbors, no position, and connections to neighbors are uni-directional.
 * \note To make a connection go both ways, define the connection for both nodes!
 */
class SimplePathNode
{
public:
	/**
	 * \param _id An id for this pathing node.
	 * \note The id should be unique in the set of nodes this node is used with, otherwise algorithms using the node will get very annoyed!
	 */
	SimplePathNode(int _id);
	~SimplePathNode();
	
	/**
	 * \brief Connect this node to another node.
	 * \param _leads_to The node that should be reachable from this node
	 * \note The connection is uni-directional!
	 */
	void ConnectTo(SimplePathNode *_leads_to);

	/**
	 * \brief Checks whether a node is reachable from this node in one step
	 * \param targetnode The node to check for reachability
	 * \return true if targetnode is DIRECTLY reachable from this node
	 */
	bool IsConnectedTo(SimplePathNode *targetnode);

	/**
	 * \return A reference to a vector containing all direct neighbors of this node
	 */
	vector<SimplePathNode*> &GetConnected() { return leads_to; };

	/**
	 * \return The id of this node
	 */
	int GetId(){ return id; };

private:

	int id;									//!< Stores the id of this node
	vector<SimplePathNode*> leads_to;		//!< Stores pointers to all neighbors
};

