#pragma once

class CoGHasChangedEvent;
/* the CoGmanager class handles the CoG shifting of a vessel. There is only supposed to be one instance per vessel
 * It is handled in a separate class more or less just for oversight */
class IMS_CoGmanager :
	public IMS_Manager_Base
{
public:
	IMS_CoGmanager(IMS2 *_vessel);
	~IMS_CoGmanager();

	/**
	 * \brief calculates and sets the CoG of the vessel based on its modules
	 * \note Will fire a CoGHasChangedEvent
     */
	void SetCoG();
	
	/**
	 * \brief calculates and sets the vessels total mass
	 */
	void SetMass();

	/* returns the current CoG position of the vessel relative to the core module
	*/
	VECTOR3 GetCoG() { return cogpos; };
	
	/* initialises the center of gravity if the original
	 * center of gravity is not at (0, 0, 0) of the vessel
	 * should only be called from clbkLoadStateEx()
	 */
	void InitialiseCoG(VECTOR3 cog);

	void PostLoad() { SetMass(); };


private:
	IMS2 *vessel;
	VECTOR3 cogpos;

	//calculates the CoG of the vessel from scratch
	VECTOR3 calculateNewCoG();
	/* returns a vector that contains the modules that do not have a symmetric counterpart in one axis (x or y)
	 * DOES NOT RECOGNISE RADIAL SYMMETRY! used for avoiding unnecessary calculations that lead to double-inprecisions whe calculating CoG
	 * to remove symmetries in multiple axes, you need to call this method multiple times
	 * symmetricallysortedmodules: a vector of modules sorted in a way that symmetrically located modules are located next to each other
	 */
	vector<IMS_Module*> removeSymmetricModules(vector<IMS_Module*> &symmetricallysortedmodules);

	bool ProcessEvent(Event_Base *e);
};

