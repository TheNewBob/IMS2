#include "Common.h"
#include "Events.h"
#include "IMS_Movable.h"
#include "IMS_Location.h"

const LOCATION_CONTEXT IMS_Location::CONTEXT_PRESSURISED = LOCATION_CONTEXT(0, "pressurised");
const LOCATION_CONTEXT IMS_Location::CONTEXT_VACUUM = LOCATION_CONTEXT(1, "vacuum");
const LOCATION_CONTEXT IMS_Location::CONTEXT_NONTRAVERSABLE = LOCATION_CONTEXT(2, "not traversable");

const map<int, LOCATION_CONTEXT> IMS_Location::CONTEXTMAP = {
	{ 0, IMS_Location::CONTEXT_PRESSURISED },
	{ 1, IMS_Location::CONTEXT_VACUUM },
	{ 2, IMS_Location::CONTEXT_NONTRAVERSABLE } };

IMS_Location::IMS_Location(vector<LOCATION_CONTEXT> contexts)
	: contexts(contexts)
{
}


IMS_Location::~IMS_Location()
{
}


void IMS_Location::AddMovable(IMS_Movable *movable)
{
	Olog::assertThat([movable, this]() { return find(movables.begin(), movables.end(), movable) == movables.end(); }, "trying to add a movable that is already in the module!");		
	//tell the movable to connect its event handler
	movable->addTo(this);
	//add it to the list
	movables.push_back(movable);
}

void IMS_Location::RemoveMovable(IMS_Movable *movable)
{
	vector<IMS_Movable*>::iterator i = find(movables.begin(), movables.end(), movable);
	Olog::assertThat([i, this]() { return i != movables.end(); }, "trying to remove a movable that isn't here!");
	//tell the movable to disconnect its event handler
	movable->removeFrom(this);
	//remove it from the list
	movables.erase(i);
	
}


void IMS_Location::InvokeMovablePreStep(double simdt)
{
	for (UINT i = 0; i < movables.size(); ++i)
	{
		movables[i]->PreStep(this, simdt);
	}
}

double IMS_Location::GetMovableMass()
{
	double totalmass = 0.0;
	for (UINT i = 0; i < movables.size(); ++i)
	{
		totalmass += movables[i]->GetMass();
	}
	return totalmass;
}


bool IMS_Location::HasLocationContext(int contextId)
{
	for (UINT i = 0; i < contexts.size(); ++i)
	{
		if (contexts[i].id == contextId) return true;
	}
	return false;
}

bool IMS_Location::HasAnyOfLocationContexts(vector<int> &contextIds)
{
	for (UINT i = 0; i < contexts.size(); ++i)
	{
		if (find(contextIds.begin(), contextIds.end(), contexts[i].id) != contextIds.end()) return true;
	}
	return false;
}

void IMS_Location::AddLocationContext(int contextId)
{
	Olog::assertThat([&]() { return !HasLocationContext(contextId); },
		"Adding context to location that it already has!");
	contexts.push_back(CONTEXTMAP.find(contextId)->second);
}

void IMS_Location::RemoveLocationContext(int contextId)
{
	bool contextFound = false;
	for (UINT i = 0; i < contexts.size(); ++i)
	{
		if (contexts[i].id == contextId)
		{
			contexts.erase(contexts.begin() + i);
			contextFound = true;
			break;
		}
	}
	Olog::assertThat([&]() { return !contextFound;  }, "Removing non-existing context from location!");
}
