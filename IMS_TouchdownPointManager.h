#pragma once

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

class BoundingBox;

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

private:
	//generic settings for hullpoints. Will need some experimentation to get right
	//adjust in IMS_TouchdownPointManager.cpp
	static const double TD_STIFFNESS;
	static const double TD_LATFRICTION;
	static const double TD_DAMPING;
	static const double TD_LONGFRICTION;

	BoundingBox *hullbox = NULL;		//axis-aligned bounding box of the entire vessels hull.
	vector<TOUCHDOWNVTX> hullpoints;	//contains the full list of touchdown points currently on the vessel, but with absolute positions.
	vector<SHAPEDATA> shapes;			//contains pointers to all shapes the vessel is composed of.

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
	 * \brief Creates the default 3 touchdownpoints either from the vessels landing gear,
	 *	or from the bounding box of the hull if there is no landing gear.
	 * \note These points must always be the first 3 points in the list of touchdown points
	 *	passed to orbiter, or there will be mayhem!
	 * \note The received vertices will already be CoG-relative!
	 */
	void getDefaultTdPoints(vector<TOUCHDOWNVTX> &OUT_points);
};

