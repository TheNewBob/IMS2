#pragma once

/**
 * \file StateMachine.h
 * A dynamic state machine that handles states as connected nodes on a
 * map, capable to find the shortest way to move from one state to another
 * using a breadth-first search. 
 * 
 * The machine uses a concept of Stable States and Intermediate states
 * To judge when a state can be skipped.
 *
 * A stable state is one that lasts indefinitely if there is no input,
 * but can switch to another state at the drop of a hat.
 * for example, a folded up piece of equipment is in a stable state:
 * nothing happens with it until it is told to unfold, and if told to
 * do so, it can start immediately without waiting for other critical
 * actions to finish.
 *
 * An intermediate state is a state that will eventually change without
 * outside input, but can only do so when it is "finished".
 * for example, a piece of equipment in the process of unfolding will
 * eventually have unfolded without further outside input. However, until 
 * it has done so, it cannot progress to other states that need it to be
 * unfolded first.
 *
 * Most commonly, stable states are connected together by an intermediate states,
 * but this is by no means an iron rule. For example, while an unfolding piece of
 * equipment cannot do anything that needs it to be unfolded first until it has
 * finished the unfolding state, but it could start packing itself up again before
 * finishing.
 *
 * This boils down to two simple rules:
 * Stable states are always save to skip, even if several in a row.
 * Intermediate states are save to skip if they need to change into
 * another intermediate state.
 *
 * It is the job of the programer that sets up the machine to ensure that his states are
 * set up in a proper way, that he informs the machine of the state it needs to move to,
 * and that he properly communicates events that signal the completion of intermediate states.
 * The rest will be handled by the machine.
 */


class StateNode;


class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	/**
	 * \brief Adds a new state to the statemachine 
 	 * \param id an id unique in this state machine that identifies the state
	 * \param description a text description of this state
	 * \param is_stable whether the state is considered stable or not
	 */
	void AddState(int id, string description = "", bool is_stable = true);

	
	/**
	 * \brief Sets the initial state of the statemachine
	 *
	 * This function can only be called once in the lifetime of the machine
	 * This function has to be called before getting the state, setting a
	 * target state or advancing the state.
	 * \param id: the id of the state the machine should have when it's first used
	 */
	void SetInitialState(int id);

	/**
	 * \brief Connects one state to another, telling the StateMachine that the target state
	 * can be reached directly from this state.
	 *
	 * \note UNIDIRECTIONAL from source to target! If the connection is bidirectional, 
	 *	call this function twice with inverted source and target.
	 * \param source_state_id the id of the state at the root of the connection
	 * \param target_state_id the id of the state to which the source state should point to
	 */
	void ConnectStateTo(int source_state_id, int target_state_id);

	/**
	 * \brief Returns the description of the current state
	 * \note If you are using GetStateAndAdvance(), call this first, as this	
	 * can advance the state after its been called.
	 * \see GetStateAndAdvance()
	 */
	string GetStateDescription();

	/**
	 * \return The state the machine is currently in, without changing it
	 * \see GetStateAndAdvance()
	 */
	int GetState();

	/**
	 * \brief Returns the current state and immediately advances to the next secure state.
	 * \return The state the machine is currently in.
	 * \note Don't call this function twice in the same frame! Usually you'll want to use GetState()
	 * \see GetState()
	 * \see AdvanceStateSecure()
	 */
	int GetStateAndAdvance();

	/** 
	 * \return True if the current state is a stable state,
	 * false if it is intermediate.
	 */
	bool IsStable();

	/**
	 * \return The currently set taget state
	 * \note Returns the current state if no target state is set
	 */
	int GetTargetState();

	/**
	 * \brief Sets the state the StateMachine should be moving towards
	 * \param target_id The id of the targeted state
	 */
	void SetTargetState(int target_id);

	/**
	 * \brief Tells the machine that the current state should be advanced.
	 *
	 * The state is advanced by exactly one towards the desired state. If the current state is the desiredstate,
	 * this call has no effect other than returning the id of the current state.
	 * \return the new current state of the statemachine.
	 * 
	 * \note This call does not discriminate between stable and intermediate states and
	 *	has the potential to throw the machine out of synch with its environment.
	 *	It should only be called on events that terminate an intermediate state,
	 *	never on user input!
	 */
	int AdvanceState();

	/**
	 * \brief Tells the machine that the current state should be advanced
 	 * along the targeted path by as many states as are save. 
	 *
	 * This boils down to three simple rules:
	 * The state will not be advanced if the current state is the target state.
	 * If the current state is stable, the state will be advanced to the next
	 * intermediate state on the path, skipping any other stable states until there.
	 * If the current state is intermediate, the state will advance if the next state
	 * is also intermediate. If the next state is stable, it will NOT advance an
	 * intermediate state!
	 * \see  GetStateAndAdvance()
	 */
	int AdvanceStateSecure();


	/**
	 * \brief Checks if the state has changed since the last call to this function.
	 * \return True if the state has changed, false otherwise.
	 * \note This function is intended to be used in control loops that 
	 *	call it in regular intervals to check if the state has changed.
	 */
	bool StateChanged();

	/**
	 * \return true as long as current state and
	 * target state are identical, false if not 
	 */
	bool StateReached();


	/**
	 * \return True if the statemachine had its initial state set, false otherwise
	 */
	bool IsInitialised() { return path.size() != 0; };


private:
	map<int, StateNode*> states;			//!< maps all states by their id
	bool statechanged = false;				//!< Stores if the state has changed since the last call to StateChanged()

	/**
	 * Contains the path from the current state(front of the dequeue) to the desired state (back of the deque)
	 * if the desired state and the current state are equal, it contains only the current state.
	 */
	deque<StateNode*> path;			

	/**
	 * \brief Checks whether the current state is save to skip
	 * \return True if the current state is save to skip
	 */
	bool isStateSaveToSkip();

};

