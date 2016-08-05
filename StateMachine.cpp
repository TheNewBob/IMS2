#include "Common.h"
#include <deque>
#include <queue>
#include <stack>
#include "SimpleTreeNode.h"
#include "SimplePathNode.h"
#include "StateNode.h"
#include "StateMachine.h"
#include "PathFinding.h"


StateMachine::StateMachine()
{
}


StateMachine::~StateMachine()
{
	for (map<int, StateNode*>::iterator i = states.begin(); i != states.end(); ++i)
	{
		delete i->second;
	}
	states.clear();
}


void StateMachine::AddState(int id, string description, bool is_stable)
{
	assert(states.find(id) == states.end() &&
		"You're trying to add the same state twice!");

	assert(path.size() == 0 &&
		"A current state has already been set for this state machine!");

	StateNode *node = new StateNode(id, is_stable, description);
	states[id] = node;
}


void StateMachine::SetInitialState(int id)
{
	assert(path.size() == 0 &&
		"you can set the initial state only once!");

	assert(states.find(id) != states.end() &&
		"you are trying to set a non-existing state as initial state!");
	
	path.push_front(states[id]);
}


void StateMachine::ConnectStateTo(int source_state_id, int target_state_id)
{
	assert(states.find(source_state_id) != states.end() &&
		states.find(target_state_id) != states.end() &&
		"one or both of the states you're trying to connect does not exist!");

	states[source_state_id]->ConnectTo(states[target_state_id]);
}


string StateMachine::GetStateDescription()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	return path.front()->GetDescription();
}

int StateMachine::GetState()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	return path.front()->GetId();
}

int StateMachine::GetStateAndAdvance()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	int currentstate = path.front()->GetId();
	AdvanceStateSecure();
	return currentstate;
}

int StateMachine::GetTargetState()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state");
	return path.back()->GetId();
}

bool StateMachine::IsStable()
{
	assert(path.size() > 0 &&
		"State Machine does not have current state (forgot to set initial state?");

	return path.front()->IsStable();
}




int StateMachine::AdvanceState()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	if (path.size() > 1)
	{
		//the current state is not the target state, advance the current state by one step.
		path.pop_front();
		statechanged = true;
	}
	return path.front()->GetId();
}


int StateMachine::AdvanceStateSecure()
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	//intermediate states are considered save to skip if they lead to another intermedite state,
	//but not if they lead to a stable state.
	if (isStateSaveToSkip())
	{
		path.pop_front();
		statechanged = true;
	}

	return path.front()->GetId();
}



void StateMachine::SetTargetState(int target_id)
{
	assert(path.size() > 0 &&
		"State Machine does not know initial state!");

	assert(states.find(target_id) != states.end() &&
		"You're trying to target a state that doesn't exist!");

	StateNode *targetstate = states[target_id];
	
	stack<int> pathids;					//used to store the path found by the algorithm
	bool pathfindingsuccess = PathFinding::BreadthFirst(path.front(), targetstate, pathids);
	assert(pathfindingsuccess);

	//now recreate the path from the ids returned in pathids
	path.clear();
	while (pathids.size() > 0)
	{
		path.push_back(states[pathids.top()]);
		pathids.pop();
	}
}


bool StateMachine::StateChanged()
{
	assert(path.size() > 0 &&
		"No initial state defined!");
	if (statechanged)
	{
		statechanged = false;
		return true;
	}
	return false;
}

bool StateMachine::StateReached()
{
	assert(path.size() > 0 &&
		"No initial state defined!");

	if (path.size() == 1)
	{
		return true;
	}
	return false;
}


bool StateMachine::isStateSaveToSkip()
{
	//first, check if the current state is the desired state.
	//if it is, we can't skip it under any circumstances
	if (path.size() > 1)
	{
		//intermediate states are considered save to skip if they lead to another intermedite state,
		//but not if they lead to a stable state.
		if (!path.front()->IsStable() && !path[1]->IsStable())
		{
			return true;
		}

		//stable states lying on the path are always save to skip
		if (path.front()->IsStable())
		{
			return true;
		}
	}

	return false;
}





