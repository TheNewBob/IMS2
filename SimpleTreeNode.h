#pragma once
/**
 * \brief The SimpleTreeNode is just what it says on the tin... a very simple node to form a tree structure.
 *
 * It is used by the breadth-first algorithm to put its results in a nice tree structure where the 
 * shortest path can be traced back without effort.
 * Note that this structures single purpose is to PRESERVE order and connection of elements among
 * each other. It makes for a lousy search and sorting structure, and therefore has no functions
 * to facilitate these things. Use a map if you need a search tree.
 */
class SimpleTreeNode
{
public:
	/**
	 * \brief Constructs a node with an id and inserts it into a tree
	 *
	 * The node's id and position in the tree are fixed and can never change!
	 * \param _id The id for the node. Must be unique in the entire tree
	 * \param _parent The parent node this node should be inserted after.
	 * \note If the node you are creating is a tree's root, pass NULL for _parent. 
	 */
	SimpleTreeNode(int _id, SimpleTreeNode *_parent) : id(_id), parent(_parent)
	{
		if (parent != NULL)
		{
			parent->addChild(this);
		}
	};

	/**
	 * \brief Recursively deletes the entire tree when called on the root.
	 *
	 * will trigger an assert if NOT called on the root!
	 */
	~SimpleTreeNode()
	{
		Helpers::assertThat([this]() { return parent == NULL; },
			"Delete a SimpleTree at the root, otherwise you create a memory leak!");

		for (UINT i = 0; i < children.size(); ++i)
		{
			children[i]->parent = NULL;
			delete children[i];
		}
	};

	/**
	 * \return The parent of this node
	 */
	SimpleTreeNode *GetParent() { return parent; };
	
	/**
	 * \return The id of this node
	 */
	int GetId() { return id; };

private:
	vector<SimpleTreeNode*> children;				//!< Stores all children of this node
	SimpleTreeNode *parent = NULL;					//!< Stores the parent of this node
	int id;											//!< Stores the id of this node

	/**
	 * \brief Adds a child to this node. Done automatically when a childnode is created
	 * \param child The node to add as a child
	 */
	void addChild(SimpleTreeNode *child){ children.push_back(child); };

};

