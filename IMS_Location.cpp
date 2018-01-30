#include "Common.h"
#include "Events.h"
#include "IMS_Movable.h"
#include "IMS_Location.h"


IMS_Location::IMS_Location()
{
}


IMS_Location::~IMS_Location()
{
}


void IMS_Location::AddMovable(IMS_Movable *movable)
{
	Helpers::assertThat([movable, this]() { return find(movables.begin(), movables.end(), movable) == movables.end(); }, "trying to add a movable that is already in the module!");		
	//tell the movable to connect its event handler
	movable->addTo(this);
	//add it to the list
	movables.push_back(movable);
}

void IMS_Location::RemoveMovable(IMS_Movable *movable)
{
	vector<IMS_Movable*>::iterator i = find(movables.begin(), movables.end(), movable);
	Helpers::assertThat([i, this]() { return i != movables.end(); }, "trying to remove a movable that isn't here!");
	//tell the movable to disconnect its event handler
	movable->removeFrom(this);
	//remove it from the list
	movables.erase(i);
}


void IMS_Location::InvokeMovablePreStep()
{
	for (UINT i = 0; i < movables.size(); ++i)
	{
		movables[i]->PreStep(this);
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