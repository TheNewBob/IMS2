#pragma once

class SimpleShape;
class BoundingBox;

/**
 * \brief Contains the data necessary to place the hullshape of a module into
 *	the overall hullshape of a vessel.
 */
struct SHAPEDATA
{
	SimpleShape *shape;
	VECTOR3 pos;
	MATRIX3 rot;

	SHAPEDATA(SimpleShape *_shape, VECTOR3 &_pos, MATRIX3 &_rot)
	{
		shape = _shape;
		pos = _pos;
		rot = _rot;
	}

	bool operator==(SimpleShape *shape)
	{
		return this->shape == shape;
	}
};

/**
 * \brief remembers vertex and direction of currently existing landing gear points
 */
struct TDDATA
{
	TDDATA(){};

	TDDATA(TOUCHDOWNVTX vtx, VECTOR3 dir)
	{
		this->vtx = vtx;
		this->dir = dir;
	};

	TOUCHDOWNVTX vtx;
	VECTOR3 dir;
};


class IMS_TouchdownPointManager :
	public IMS_Manager_Base
{
public:
	IMS_TouchdownPointManager(IMS2 *vessel);
	~IMS_TouchdownPointManager();

	/**
	 * \brief Adds a new shape to the overall hullshape of the vessel
	 * \param shape The shape to be included in the overall hullshape
	 * \param pos The origin position of the shape, vessel-relative (in ABSOLUTE vessel coordinates, not relative to the current CoG!)
	 * \param rot The rotation matrix that describes the modules rotation relative to the vessel.
	 */
	void AddHullShape(SimpleShape *shape, VECTOR3 pos, MATRIX3 rot);

	/**
	 * \brief Removes a shape from the overall hullshape
	 * \param shape The shape to be removed from the hull.
	 */
	void RemoveHullShape(SimpleShape *shape);

	/**
	 * \brief Adds a touchdown point belonging to an extended landing gear
	 * \param pos The position of the touchdown point in vessel-absolute coordinates.
	 * \param dir Normalised directional vector describing the down-direction of the TD-point, vessel-relative.
	 * \param stiffness The stiffness of the touchdown point (see orbiter reference for details).
	 * \param damping The damping coefficient of the touchdown point (see orbiter reference for details).
	 * \return An identifier by which the manager remembers this point. Whoever created the touchdownpoint
	 *	will need to pass this identifier to remove it again when the gear retracts.
	 */
	UINT AddLandingTdPoint(VECTOR3 &pos, VECTOR3 &dir, double stiffness, double damping);

	/**
	 * \brief removes a touchdown point belonging to a landing gear.
	 * \param id The identifier of the touchdownpoint received by AddLandingTdPoint().
	 */
	void RemoveLandingTdPoint(UINT id);

	void PostLoad();

	/**
	 * \brief Turns placement assistance for scenario editor on or off.
	 * \note This works by giving appropriate stiffness and damping to the 3 default touchdown points
	 *	defining the ground-contact plane, because these are the only ones scened cares about. 
	 *	Having this active can lead to inconsistent td-point behavior during normal operation.
	 */
	void SetScenedAssistance(bool enabled);

private:
	//generic settings for hullpoints. Will need some experimentation to get right
	//adjust in IMS_TouchdownPointManager.cpp
	static const double LTD_STIFFNESS;
	static const double LTD_DAMPING;
	static const double TD_STIFFNESS;
	static const double TD_LATFRICTION;
	static const double TD_DAMPING;
	static const double TD_LONGFRICTION;

	BoundingBox *hullbox = NULL;		//!< axis-aligned bounding box of the entire vessels hull.
	vector<TOUCHDOWNVTX> hullpoints;	//!< contains the full list of touchdown points currently on the vessel, but with absolute positions.
	vector<SHAPEDATA> shapes;			//!< contains pointers to all shapes the vessel is composed of.
	map<UINT, TDDATA> landingpoints;	//!< contains the positions of all td points belonging to extended landing gear, maped to an int identifier.
	
	/**
	 * the range in which ids have been handed out, to avoid duplication. It's a bit lazy, 
	 * it would be better to free ids that no longer exist, but I don't expect anybody to add 32 bits worth
	 * of landing gear to a vessel over the course of construction (you hear me Dantassii?)
	 */
	UINT landingpoint_id = 0;		

	/**
	 * \brief Sets the touchdown points on the orbiter vessel
	 */
	void setTdPoints();

	virtual bool ProcessEvent(Event_Base *e);
	
	/**
	 * \brief internal implementation of AddHullShape(). Calling this internally will add a shape to the touchdownpoints list,
	 *	without adding the shape to the shapes list, as it can be asumed that the shape is already on that list.
	 * \param shape The shape to be included in the overall hullshape
	 * \param pos The origin position of the shape, vessel-relative (in ABSOLUTE vessel coordinates, not relative to the current CoG!)
	 * \param rot The rotation matrix that describes the modules rotation relative to the vessel.
	 */
	void addHullShape(SimpleShape *shape, VECTOR3 pos, MATRIX3 rot);

	/**
	 * \brief Creates the default 3 touchdownpoints based on the vessels hullshape
	 * More precisely, the 3 generated points will lie on the center-front, rear-left and rear-right
	 * of the bottom(0 -1 0) rectangle of the hullshape's bounding box.
	 * \returns A vector with size 3.
	 */
	vector<VECTOR3> createDefaultTdTriangleFromHullshape();

	
	/**
	 * \brief Creates the default triangle of attachment points for this vessel.
	 * If there are 3 or more landing gears, these points will be calculated from the landing gears
	 * position and direction.
	 * If there are less than 3 landing gears, these points will conform to the default "bottom" (0 -1 0) of the hullshape
	 * If there is no hullshape, the points created by orbiter will be used.
	 * \todo this will return coordinates CoG-relative, while absolute is expected. Not a problem since this usually only happens with one single
	 *	module, when both are one and the same, but might need some looking into in the future.
	 */
	void createDefaultTdTriangle();

	/**
	* \brief Creates the default triangle of attachment points for this vessels landing gear.
	* If there is no hullshape, the points created by orbiter will be used.
	* \note There must be at least 3 landing gears on the vessel for this method to work!
	* \returns A vector with size 3.
	* \note Assumes that the ground-contact plane is perpendicular to the y-axis, does not work for other arrangements!
	*/
	vector<VECTOR3> createDefaultTdTriangleFromLandingGear();

	/**
	 * \brief calculates stiffness for the first three touchdownpoints so their compression results in a leveled vessel, even when load is balanced unequally.
	 * \param IN_OUT_tdarray Array of touchdown vertices, where the first three
	 *	points already are at the correct position and will be given an appropriate stiffness.
	 * \param displacement Desired displacement (compression) of the touchdown points at 1G. Must not be zero!
	 */
	void setDefaultTdStiffness(TOUCHDOWNVTX *IN_OUT_tdarray, double displacement);

	/**
	 * \brief comparator to sort VECTOR3 by x-z distance.
	 */
	static bool SORT_BY_HORIZONTAL_DISTANCE(VECTOR3 &a, VECTOR3 &b);

	/**
	 * \brief Comparator to sort VECTOR3 by z axis in descending order
	 */
	static bool SORT_DESCENDING_BY_Z(VECTOR3 &a, VECTOR3 &b);


	vector<TOUCHDOWNVTX> defaulttdtriangle;				//!< Vector that holds the triangle of default touchdown points for this vessel

	bool firstTdPointsChangedEvent = true;				//!< Turns to false after the first TDPOINTSCHANGEDEVENT has been processed.

	bool scened_assist = false;							//! placement assistance for scenario editor.
};

