#pragma once

enum FORCETYPE
{
	F_LINEAR,
	F_TORQUE
};

/**
 * This class represents the entire RCS with all involved thrusters of a vessel
 */
class FSThrusterCollection
{
public:
	FSThrusterCollection(vector<THRUSTER_HANDLE> thrusters, VESSEL *vessel);
	
	/**
	 * \brief Constructs the collection out of already created FiringSolutionThrusters.
	 * Intended for use in unit tests.
	 * \param thrusters Vector of already constructed thrusters.
	 * \note The lifetime of thrusters must be longer than the lifetime of the collection created with this constructor!
	 */
	FSThrusterCollection(vector<FiringSolutionThruster> &thrusters);
	~FSThrusterCollection();

	/**
	 * \return A reference to a vector of pointers to all thrusters in a group, sorted in descending order by their suitability for that score
	 * \param group The group to get the thrusters of.
	 */
	vector<FiringSolutionThruster*> &GetThrustersInGroup(THGROUP_TYPE group) { return grouplists[group]; };

	
	/**
	 * \return A vector containing all thrusters appearing in any of the demanded groups AND the governing group.
	 *
	 * This method is a bit misnamed, but it's difficult to name. Basically what it does is give the intersection between
	 * the governing group and the union of the other groups. I.e. every thruster that is part of the governing group
	 * and also appears in any of the other groups.
	 * \param groups A list of groups from which the union is constructed.
	 * \param governing_group The result will be the intersection of this group and the union of groups. The resulting vector
	 *	will also be sorted by the thrusters scoring in this group.
	 */
	 vector<FiringSolutionThruster*> GetGroupUnion(vector<THGROUP_TYPE> &groups, THGROUP_TYPE governing_group);


	/**
	 * \brief Retrieves the overall forces all thrusters from this group will exert on the vessel.
	 * \param group The thruster group for which to retrieve the forces
	 * \return A map with the keys F_LINEAR and F_TORQUE, containing the corresponding force vectors
	 */
	map<FORCETYPE, VECTOR3> GetTotalForces(THGROUP_TYPE group);

	/**
	* \brief comparator to sort thrusters according to their score in the group set in sortgroup
	*/
	static bool SortByGroupScore(FiringSolutionThruster *a, FiringSolutionThruster *b);

private:
	vector<FiringSolutionThruster*> thrusterlist;

	map<THGROUP_TYPE, vector<FiringSolutionThruster*>> grouplists;
	map<THGROUP_TYPE, map<FORCETYPE, VECTOR3>> totalforces;

	void createGroupLists();

	/**
	* \brief Compares the scores of the passed thrusters and decides if there are some group assignements that don't make sense.
	*
	* A thruster will consider itself suitable to groups if it is equally useless in both of them and can't make out a clearly dominating purpose.
	* Calling this method will enable the thruster to examine itself in the bigger context. It might be that it is in fact needed in some of those
	* groups because there's noone better around for the job. But it's also possible that there are thrusters that can handle those groups a lot
	* more efficiently, in which case the thruster will remove itself from these groups.
	* \param thrusters A vector containing all thrusters to be compared, commonly all RCS thrusters on the vessel.
	*/
	void compareThrusterScores();

	/**
	 * \brief Applies final scoring to thruster suitability by the amount of groups they are assigned to
	 */
	void applyFinalScores();

	map<THGROUP_TYPE, double> getMaximumGroupScores();

	void calculateOverallForces();

	/*
	* \brief Cutoff threshold of relative thruster efficiency for group assignement. Adjust at the head of FiringSolutionThruster.cpp.
	*
	* The reference for the comparison is the score of the thruster on the vessel that is best suited
	* for this group. If another thruster in the same group falls below this score multiplied by
	* MINRELATIVEEFFICIENCY, it is considered more of a distrction than a help in the task and gets
	* unassigend from that group
	*/
	static const double MINRELATIVEEFFICIENCY;

	/**
	* SortByGroupScore will sort by the score of the group set in this member.
	* \see SetSortGroup()
	*/
	static THGROUP_TYPE sortbygroup;

	bool deletethrustersonexit = true;				//if the class has not allocated the thrusters itself, it also shouldn't delete them



};

