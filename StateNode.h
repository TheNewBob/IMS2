#pragma once

/**
 * \brief A SimplePathNode representing a state in the statemachine.
 *
 * Provides an additional member to signify whether
 * this state is stable or intermediate.
 * For a more detailed description of how the state machine works, see StateMachine.h
 * \see StateMachine.h
*/
class StateNode : public SimplePathNode
{
public:
	/**
	 * \param _id An id for this node, locally unique inside the state machine
	 * \param _stable Wether this state is stable or intermediate
	 * \param _description Optional; A text description for this state
	 */
	StateNode(int _id, bool _stable, string _description = "");
	~StateNode();

	/**
	 * \return True if this is a stable state, false otherwise
	 */
	bool IsStable(){ return stable; };

	/**
	 * \return The description of this statenode
	*/
	string GetDescription() { return description; };


private:
	bool stable;					//!< Stores whether this is a stable state or not
	string description;				//!< Stores the state's description	

};

