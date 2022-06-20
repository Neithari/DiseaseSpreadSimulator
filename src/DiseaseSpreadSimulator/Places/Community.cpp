#include "Places/Community.h"
#include <utility>
#include "Places/PlaceBuilder.h"
#include "Person/Person.h"
#include "Person/PersonPopulator.h"
#include "RandomNumbers.h"

DiseaseSpreadSimulation::Community::Community(const size_t populationSize, const Country country)
{
	// Return early and leave places and population empty with a population size of 0
	if (populationSize == 0)
	{
		return;
	}

	m_places = PlaceBuilder::CreatePlaces(populationSize, country);

	PersonPopulator populationFactory(populationSize, PersonPopulator::GetCountryDistribution(country));
	m_population = populationFactory.CreatePopulation(country, m_places.homes, m_places.workplaces, m_places.schools, this);
}

DiseaseSpreadSimulation::Community::Community(const Community& other)
	: m_population(other.m_population),
	  m_places(other.m_places),
	  m_travelLocation(other.m_travelLocation),
	  populationMutex(),
	  placesMutex()
{
}

DiseaseSpreadSimulation::Community::Community(Community&& other) noexcept
	: m_population(std::move(other.m_population)),
	  m_places(std::move(other.m_places)),
	  m_travelLocation(std::move(other.m_travelLocation)),
	  populationMutex(),
	  placesMutex()
{
}

DiseaseSpreadSimulation::Community& DiseaseSpreadSimulation::Community::operator=(const Community& other)
{
	return *this = Community(other);
}

// We don't want to copy populationMutex and placesMutex so we suppress the static analyzer warning
// cppcheck-suppress operatorEqVarError
DiseaseSpreadSimulation::Community& DiseaseSpreadSimulation::Community::operator=(Community&& other) noexcept
{
	std::swap(m_population, other.m_population);
	std::swap(m_places, other.m_places);
	std::swap(m_travelLocation, other.m_travelLocation);
	return *this;
}

void DiseaseSpreadSimulation::Community::AddPerson(Person person)
{
	person.SetCommunity(this);
	std::lock_guard<std::shared_timed_mutex> lockAddPerson(populationMutex);
	m_population.push_back(std::move(person));
}

void DiseaseSpreadSimulation::Community::RemovePerson(const Person& personToRemove)
{
	std::lock_guard<std::shared_timed_mutex> lockRemovePerson(populationMutex);
	m_population.erase(
		std::remove_if(m_population.begin(), m_population.end(), [&](const Person& person)
			{
				return person == personToRemove;
			}),
		m_population.end());
}

void DiseaseSpreadSimulation::Community::AddPlaces(Places places)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlaces(placesMutex);
	m_places.Insert(std::move(places));
}

void DiseaseSpreadSimulation::Community::AddPopulation(std::vector<Person>& population)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPopulation(populationMutex);
	m_population.reserve(m_population.size() + population.size());
	m_population.insert(m_population.end(), population.begin(), population.end());
}

std::optional<DiseaseSpreadSimulation::Person> DiseaseSpreadSimulation::Community::TransferPerson(const Person& traveler)
{
	// The shared lock can't be upgraded to a full lock. Because of that we need to separate the read from the write part.
	bool isEnditerator{true};
	std::vector<Person>::iterator toTransfer;
	{
		std::shared_lock<std::shared_timed_mutex> lockFindPerson(populationMutex);
		toTransfer = std::find_if(m_population.begin(), m_population.end(), [&](const Person& person)
			{
				return person == traveler;
			});
		isEnditerator = toTransfer == m_population.end();
	}

	if (!isEnditerator)
	{
		std::lock_guard<std::shared_timed_mutex> lockPersonTransfer(populationMutex);
		std::optional<Person> transferPerson = std::move(*toTransfer);
		m_population.erase(toTransfer);
		return transferPerson;
	}
	// This should never happen, because the person to transfer should be calling it.
	return std::nullopt;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToHome(Person* person)
{
	auto* home = person->GetHome();
	TransferToPlace(person, home);
	return home;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToSupplyStore(Person* person)
{
	auto* store = GetSupplyStore();
	TransferToPlace(person, store);
	return store;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToHardwareStore(Person* person)
{
	auto* store = GetHardwareStore();
	TransferToPlace(person, store);
	return store;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToWork(Person* person)
{
	auto* work = person->GetWorkplace();
	TransferToPlace(person, work);
	return work;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToSchool(Person* person)
{
	auto* school = person->GetSchool();
	TransferToPlace(person, school);
	return school;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToMorgue(Person* person)
{
	auto* morgue = GetMorgue();
	TransferToPlace(person, morgue);
	return morgue;
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToTravelLocation(Person* person)
{
	// Currently only one travel location
	TransferToPlace(person, &m_travelLocation);
	return &m_travelLocation;
}

std::vector<DiseaseSpreadSimulation::Person>& DiseaseSpreadSimulation::Community::GetPopulation()
{
	return m_population;
}

DiseaseSpreadSimulation::Places& DiseaseSpreadSimulation::Community::GetPlaces()
{
	return m_places;
}

DiseaseSpreadSimulation::Travel& DiseaseSpreadSimulation::Community::GetTravelLocation()
{
	return m_travelLocation;
}

std::vector<DiseaseSpreadSimulation::Home>& DiseaseSpreadSimulation::Community::GetHomes()
{
	return m_places.homes;
}

DiseaseSpreadSimulation::Supply* DiseaseSpreadSimulation::Community::GetSupplyStore()
{
	std::shared_lock<std::shared_timed_mutex> lockGetPlaces(placesMutex);
	if (m_places.supplyStores.empty())
	{
		return nullptr;
	}

	return &m_places.supplyStores.at(Random::RandomVectorIndex(m_places.supplyStores));
}

DiseaseSpreadSimulation::HardwareStore* DiseaseSpreadSimulation::Community::GetHardwareStore()
{
	std::shared_lock<std::shared_timed_mutex> lockGetPlaces(placesMutex);
	if (m_places.hardwareStores.empty())
	{
		return nullptr;
	}

	return &m_places.hardwareStores.at(Random::RandomVectorIndex(m_places.hardwareStores));
}

DiseaseSpreadSimulation::Morgue* DiseaseSpreadSimulation::Community::GetMorgue()
{
	std::shared_lock<std::shared_timed_mutex> lockGetPlaces(placesMutex);
	if (m_places.morgues.empty())
	{
		return nullptr;
	}

	return &m_places.morgues.at(Random::RandomVectorIndex(m_places.morgues));
}

void DiseaseSpreadSimulation::Community::AddPlace(Home home)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.homes.push_back(std::move(home));
}

void DiseaseSpreadSimulation::Community::AddPlace(Supply store)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.supplyStores.push_back(std::move(store));
}

void DiseaseSpreadSimulation::Community::AddPlace(Workplace workplace)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.workplaces.push_back(std::move(workplace));
}

void DiseaseSpreadSimulation::Community::AddPlace(School school)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.schools.push_back(std::move(school));
}

void DiseaseSpreadSimulation::Community::AddPlace(HardwareStore store)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.hardwareStores.push_back(std::move(store));
}

void DiseaseSpreadSimulation::Community::AddPlace(Morgue morgue)
{
	std::lock_guard<std::shared_timed_mutex> lockAddPlace(placesMutex);
	m_places.morgues.push_back(std::move(morgue));
}

const DiseaseSpreadSimulation::DiseaseContainment& DiseaseSpreadSimulation::Community::ContainmentMeasures() const
{
	return m_containmentMeasures;
}

void DiseaseSpreadSimulation::Community::TestStation(Person* person)
{
	if (TestPersonForInfection(person))
	{
		m_containmentMeasures.Quarantine(person);
	}
}

bool DiseaseSpreadSimulation::Community::TestPersonForInfection(const Person* person)
{
	if (!person->HasDisease())
	{
		return false;
	}

	// Return true when our test is inside the accuracy and false otherwise
	return Random::Percent<float>() < person->GetDisease()->GetTestAccuracy();
}

DiseaseSpreadSimulation::Place* DiseaseSpreadSimulation::Community::TransferToPlace(Person* person, Place* place)
{
	std::lock_guard<std::shared_timed_mutex> lockTransferToPlace(placesMutex);
	person->GetWhereabouts()->RemovePerson(person);
	place->AddPerson(person);
	return place;
}
