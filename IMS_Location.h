#pragma once

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
	IMS_Location();
	~IMS_Location();


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
	 * \brief invokes PostStep() on all movables currently in this location
	 * \note This method must be called every frame by someone (preferably the implementing class), 
	 *	but never more than once!
	 */
	void InvokeMovablePreStep();


protected:
	vector<IMS_Movable*> movables;			//!< contains all movables currently at this location
};

