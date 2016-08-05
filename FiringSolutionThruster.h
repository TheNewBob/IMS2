#pragma once
class FiringSolutionThruster
{
	friend class FSThrusterCollection;
public:
	/**
	 * \brief completely constructs the thruster out of a thruster handle
	 * \param thruster Handle to an orbiter thruster this FiringSOlutionThruster represents.
	 * \param vessel Pointer to the VESSEL instance thruster is a component of.
	 */
	FiringSolutionThruster(THRUSTER_HANDLE thruster, VESSEL *vessel);

	/**
	 * \brief Accepts thruster data directly and does not access the orbiter API.
	 * 
	 * Intended for use in unit tests.
	 * \param pos The position of the thruster relative to the CoG
	 * \param dir Direction of the force of thrust (i.e. opposite to the direction of the exhaust!).
	 * \param maxthrust The maximum thrust the thruster can produce.
	 */
	FiringSolutionThruster(VECTOR3 pos, VECTOR3 dir, double maxthrust);

	~FiringSolutionThruster();

	/**
	 * \return The Position of this thruster relative to the CoG
	 */
	VECTOR3 &GetPos() { return pos; };

	/*
	 * \return The Direction of thrust (Or, the opposite to the direction of the exhaust, because that's what people think of first)
	 */
	VECTOR3 &GetDir() { return dir; };

	/*
	 * \return The maximum torque this thruster can produce around x y and z, in Nm
	 */
	VECTOR3 &GetTorque() { return torque; };

	/*
	 * \return The maximum linear force the thruster can produce in x y z, in N
	 */
	VECTOR3 &GetLinearForce() { return linearforce; };

	/*
	 * \return The maximum magnitude of the thrust vector, in N
	 */
	double GetMaxThrust() { return maxthrust; };

	/**
	 * \return The handle to the actual thruster on the orbiter VESSEL instance
	 */
	THRUSTER_HANDLE GetHandle() { return thruster; };

	/**
	 * \return All maneuvering groups a thruster is considered suitable for, maped to a score how well they are suited
	 * \note Before using CompareThrusterScores() the thruster might still be assigned to some groups that would make sense
	 *	if it was the only thruster on the vessel, but not when considering the big picture.
	 */
	map<THGROUP_TYPE, double> &GetGroups() { return groupsuitability; };

	/**
	 * \return The score of this thruster for the particular group
	 */
	double GetScore(THGROUP_TYPE group);

	/**
	 * \return True if the passed thrusters are symmetric twins in the specified group
	 * 
	 * A symmetric twin in relation to a thruster group means a thruster that would induce *unwanted* torque
	 * for that group if it was running at a different strength than this thruster.
	 */
	static bool AreSymmetricTwins(THGROUP_TYPE group, FiringSolutionThruster *a, FiringSolutionThruster *b);

private:

	void mapThrusterToGroups(THRUSTER_HANDLE thruster);

	void setTranslationGroups();
	void setRotationGroups();

	THRUSTER_HANDLE thruster;
	VECTOR3 pos;
	VECTOR3 dir;
	VECTOR3 torque;
	VECTOR3 linearforce;
	double maxthrust;

	void initForces();
	
	/**
	 * \brief contains the thruster groups the thruster is suited for, and how well.
	 *
	 * The score is the magnitude of the normalised force vector in the particular axis of the thruster group.
	 * The larger it is, the more of the force vector points in the right direction, to a maximum of 1.
	 */
	map<THGROUP_TYPE, double> groupsuitability;		
	map<THGROUP_TYPE, FiringSolutionThruster*> symmetric_twins;		//!< If a thruster has a symmetric twin in a thruster group, it will be noted here.

};

