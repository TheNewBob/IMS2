#pragma once

/**
 * \brief The context of a location defines what movables can enter the location
 * A location can have multiple contexts.
 */
struct LOCATION_CONTEXT
{
public:
	LOCATION_CONTEXT(int id, string description)
		: id(id), description(description) {}
	const int id;
	const string description;
};

class IMS_Movable;
/**
 * \brief Abstract class that provides an interface for handling movables
 *
 * Anything that contains movables should be a location. Movables, at the lowest level,
 * move only between locations, without knowledge of any wider context. The implementation
 * of the location must provide a method to return the module in which the location is located, though.
 * Most commonly, certain modulefunctions will be locations.
 * \todo implement connection and pathfinding interface
 */
class IMS_Location
{
public:
	IMS_Location(vector<LOCATION_CONTEXT> contexts);
	~IMS_Location();

	// Location contexts 
	static const LOCATION_CONTEXT CONTEXT_PRESSURISED;		//!< Location has a pressurised environment suitable for human inhabitation.
	static const LOCATION_CONTEXT CONTEXT_VACUUM;			//!< Location is exposed to hard vacuum.
	static const LOCATION_CONTEXT CONTEXT_NONTRAVERSABLE;	//!< Location cannot be traversed, neither by humans or machines (e.g. because it's full of liquid hydrogen...).


	/**
	* \brief Adds a movable to this location
	*/
	void AddMovable(IMS_Movable* movable);

	/**
	* \brief removes a movable from this location
	*/
	void RemoveMovable(IMS_Movable *movable);

	/**
	 * \brief returns the module this location is a part of
	 */
	virtual IMS_Module *GetModule() = 0;

	/**
	 * \return The total mass of all movables currently at this location
	 */
	double GetMovableMass();

	/**
	 * \brief invokes PreStep() on all movables currently in this location
	 * \note This method must be called every frame by someone (preferably the implementing class), 
	 *	but never more than once!
	 */
	void InvokeMovablePreStep(double simdt);

	/**
	 * \return True if the location has the requested context.
	 */
	bool HasLocationContext(int contextId);

	/**
	 * \return True if the location has any of the required contexts
	 */
	bool HasAnyOfLocationContexts(vector<int> &contextIds);

	/**
	 * \brief Adds another context to the location
	 */
	void AddLocationContext(int contextId);

	/**
	 * \brief Removes a context from the location
	 */
	void RemoveLocationContext(int contextId);


protected:
	vector<IMS_Movable*> movables;			//!< contains all movables currently at this location
	vector<LOCATION_CONTEXT> contexts;

private:
	static const map<int, LOCATION_CONTEXT> CONTEXTMAP;
};

