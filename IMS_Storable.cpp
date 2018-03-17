#include "Common.h"
#include "GUIincludes.h"
#include "Events.h"
#include "Moduletypes.h"
#include "IMS_ModuleFunctionData_Base.h"
#include "IMS_ModuleFunctionData_Location.h"
#include "IMS_ModuleFunctionData_Pressurised.h"
#include "ModuleFunctionIncludes.h"
#include "IMS_Location.h"
#include "IMS_ModuleDataManager.h"
#include "IMS_Movable.h"
#include "IMS_Storable.h"
#include "Calc.h"
#include <iomanip>

IMS_Storable::IMS_Storable(double volume, CONSUMABLEDATA *contents, IMS_Location *module, double initial_mass)
	: IMS_Movable(module)
{
	this->volume = volume;
	consumable = contents;
	capacity = Calc::Round(volume * consumable->density, 1000);
	if (initial_mass == -1)
	{
		initial_mass = capacity;
	}

	Olog::assertThat([this, initial_mass]() { return initial_mass <= capacity; }, "trying to initialise with more mass than the storable can carry");

	mass = initial_mass;
}

IMS_Storable::IMS_Storable(string serialized_storable, IMS_Location *location)
	: IMS_Movable(location)
{
	vector<string> tokens;
	Helpers::Tokenize(serialized_storable, tokens, " :");
	if (tokens.size() != 8)
	{
		Olog::error("Invalid serialization string for storable in scenario: %s, aborting simulation!", serialized_storable.data());
		throw invalid_argument("IMS failed to load from scenario, see log for details");
	}
	
	consumable = IMS_ModuleDataManager::GetConsumableData(IMS_ModuleDataManager::GetConsumableId(tokens[1]));
	volume = Helpers::stringToDouble(tokens[3]);
	capacity = Calc::Round(volume * consumable->density, 1000);

	mass = Helpers::stringToDouble(tokens[5]);
	available = (bool)Helpers::stringToInt(tokens[7]);
}

IMS_Storable::~IMS_Storable()
{

}

double IMS_Storable::Fill()
{
	double previousmass = mass;
	mass = capacity;
	addEvent(new MassHasChangedEvent);
	addEvent(new ConsumableAddedEvent(previousmass));
	return capacity - previousmass;
}


double IMS_Storable::AddContent(double amount_kg, int consumable_id)
{
	//verify consumable type
	if (consumable_id != consumable->id)
	{
		Olog::warn("Attempt to add incompatible consumable to storable!");
		return 0.0;
	}

	if (mass < capacity)
	{
		double previousmass = mass;
		mass += amount_kg;

		//verify that the added amount can actually be placed
		if (mass > capacity)
		{
			double addedmass = amount_kg - (mass - capacity);
			mass = capacity;
			return addedmass;
		}
		addEvent(new MassHasChangedEvent());
		addEvent(new ConsumableAddedEvent(previousmass));
	}
	else
	{
		return 0.0;
	}

	return amount_kg;
}


double IMS_Storable::RemoveContent(double amount_kg, int consumable_id)
{
	//verify consumable type
	if (consumable_id != consumable->id)
	{
		Olog::warn("Attempt to remove incompatible consumable from storable!");
		return 0.0;
	}

	//if the mass is already zero, nothing will be removed anyways
	if (mass > 0.0)
	{
		mass -= amount_kg;

		//verify that the requested ammount can actually be removed
		if (mass < 0)
		{
			double removedmass = amount_kg + mass;
			mass = 0.0;
			return removedmass;
		}
		//fire an event to inform the module that the mass has changed
		addEvent(new MassHasChangedEvent());
	}
	else
	{
		return 0.0;
	}

	return amount_kg;
}


void IMS_Storable::PreStep(IMS_Location *location, double simdt)
{
	sendEvents();
}


string IMS_Storable::Serialize()
{
	stringstream ss; 
	ss << fixed << setprecision(3);
	ss << "T:" << consumable->shorthand << " V:" << volume << " M:" << mass << " A:" << (int)available;
	return ss.str();
}

bool IMS_Storable::ProcessEvent(Event_Base *e)
{
	return false;
}

string IMS_Storable::GetConsumableName()
{
	return consumable->name;
}