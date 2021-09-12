#include "pch.h"
#include "Places/Places.h"
#include "IDGenerator/IDGenerator.h"

uint32_t DeseaseSpreadSimulation::Place::GetID() const
{
	return placeID;
}

void DeseaseSpreadSimulation::Place::AddPerson(Person& person)
{
	people.push_back(person);
}

void DeseaseSpreadSimulation::Place::RemovePerson(uint32_t id)
{
	people.erase(
		std::remove_if(people.begin(), people.end(),
			[&](Person& person) { return person.GetID() == id; })
	);
}

const std::vector<DeseaseSpreadSimulation::Person&>& DeseaseSpreadSimulation::Place::GetPeople() const
{
	return people;
}

size_t DeseaseSpreadSimulation::Place::GetPersonCount() const
{
	return people.size();
}

DeseaseSpreadSimulation::Place::Place(uint32_t id)
	:
	placeID(id)
{
}

DeseaseSpreadSimulation::Home::Home()
	:
	Place(IDGenerator::IDGenerator<Home>::GetNextID())
{
}

std::string DeseaseSpreadSimulation::Home::GetTypeName() const
{
	std::string name = "Home" + std::to_string(GetID());
	return name;
}

DeseaseSpreadSimulation::Supply::Supply()
	:
	Place(IDGenerator::IDGenerator<Supply>::GetNextID())
{
}

std::string DeseaseSpreadSimulation::Supply::GetTypeName() const
{
	return "Supply" + std::to_string(GetID());
}

DeseaseSpreadSimulation::Work::Work()
	:
	Place(IDGenerator::IDGenerator<Work>::GetNextID())
{
}

std::string DeseaseSpreadSimulation::Work::GetTypeName() const
{
	return "Work" + std::to_string(GetID());
}

DeseaseSpreadSimulation::HardwareStore::HardwareStore()
	:
	Place(IDGenerator::IDGenerator<HardwareStore>::GetNextID())
{
}

std::string DeseaseSpreadSimulation::HardwareStore::GetTypeName() const
{
	return "HardwareStore" + std::to_string(GetID());
}

DeseaseSpreadSimulation::Morgue::Morgue()
	:
	Place(IDGenerator::IDGenerator<Morgue>::GetNextID())
{
}

std::string DeseaseSpreadSimulation::Morgue::GetTypeName() const
{
	return "Morgue" + std::to_string(GetID());
}
