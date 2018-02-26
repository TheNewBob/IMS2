#pragma once


class IMS_Storable : public IMS_Movable
{
public:

	/**
	 * \param volume The volume of the storable in m^3
	 * \param contents The data describing the consumable stored in this storable
	 * \param module The module the storable is initially located in
	 * \param initial_mass How much the storable contains after initialisation. Pass -1 to create the storable filled up.
	 * \param fixed Whether the storable can be moved or not.
	 */
	IMS_Storable(double volume, CONSUMABLEDATA *contents, IMS_Location *location, double initial_mass = -1);

	/**
	 * \brief Creates a storable from a serialized string.
	 */
	IMS_Storable(string serialized_storable, IMS_Location *location);

	~IMS_Storable();
	
	virtual void PreStep(IMS_Location *location, double simdt);

	/**
	 * \brief Adds an ammount of a certain consumable to the storable
	 * \param amount_kg The ammount to add in kg
	 * \param consumable_id The id of the consumable being added
	 * \return The amount that was actually added to the storable. This can be lower than the intended amount because of lacking storage capacity.
	 * \note consumableid is demanded for verification. If it does not  match the contents, 0 will be returned and nothing is added.
	 */
	double AddContent(double amount_kg, int consumable_id);

	/**
	 * \brief fills the storable to maximum capacity.
	 * \return The amount of mass that has been filled up.
	 */
	double Fill();

	/**
	* \brief Removes an ammount of a certain consumable from the storable
	* \param amount_kg The ammount to remove in kg
	* \param consumable_id The id of the consumable being removed
	* \return The amount that was actually removed. This can be lower than the requested amount because the storable might not have enough contents left.
	* \note consumableid is demanded for verification. If it does not  match the contents, 0 will be returned and nothing is removed.
	*/
	double RemoveContent(double amount_kg, int consumable_id);

	/**
	 * \return the id of the consumable contained by this storable
	 */
	int GetConsumableId() { return consumable->id; };

	/**
	 * Returns the name of the consumable stored in the storable
	 */
	string GetConsumableName();

	/**
	 * \return True if the passed id matches the id of this consumable
	 */
	bool operator==(int consumableid) { return consumable->id == consumableid; };

	/**
	 * \return True if the storable is available or consumption
	 */
	bool IsAvailable() { return available; };

	/**
	 * \brief Sets whether this storable is available for consumption
	 * \param isavailable True if this storable can be freely consumed, false if it is to be restricted
	 */
	void SetAvailable(bool isavailable) { available = isavailable; };

	/**
	* \return The maximum capacity of this storable in kg
	*/
	double GetCapacity() { return capacity; };

	/**
	 * \return a string describing the serialised storable, to be inserted into a scenario
	 */
	string Serialize();

protected:
	double volume;							//!< storage volume in m^3
	double capacity;						//!< max storage capacity in kg, derived from volume. Only stored to boost performance.
	CONSUMABLEDATA *consumable;				//!< Identifier of the consumable stored by this storable
	bool available = true;					//!< whether the storable is available for consumption. The availability of a storable can be restricted by the user to declare consumables off-limits

	virtual bool ProcessEvent(Event_Base *e);
};