#pragma once
/**
 * Class containing algorithms for pathfinding or related tasks
 */
class PathFinding
{
public:
	PathFinding();
	~PathFinding();

	/**
	 * \brief A breath-first solution that finds the guaranteed shortest path in terms of nodes covered.
	 *
	 * Meaning, the path from origin to target that has the fewest nodes in it, ignoring all other costs.
	 * \param origin The node to start at
	 * \param target The node to find a path to
	 * \param OUT_path Will contain the path from origin to target if the function was successful. Old contents will be overwritten!
	 * \note OUT_path will only contain the ids, not the elements itself. The stack will be empty if no path was found.
	 * \return False if no path exists, true otherwise
	 * \note In case origin and target are the same node, the algorithm will search for a loopback path and return the shortest
	 *	path leading from the origin node to itself, instead of not searching for a path at all.
	 *  Since paths between nodes must not necessarily be traversible in both directions, this is more useful than it sounds.
	 */
	static bool BreadthFirst(SimplePathNode *origin, SimplePathNode *target, stack<int> &OUT_path);
};

