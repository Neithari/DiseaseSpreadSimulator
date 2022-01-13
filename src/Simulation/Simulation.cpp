#include "pch.h"
#include "Simulation/Simulation.h"
#include "Places/CommunityBuilder.h"

DeseaseSpreadSimulation::Simulation::Simulation(uint16_t populationSize, bool withPrint)
	:
	withPrint(withPrint),
	populationSize(populationSize)
{
}
void DeseaseSpreadSimulation::Simulation::Run()
{
	//SetupEverything();

	while (!stop)
	{
		while (pause)
		{
			// Idle when pause is called until resumed
		}

		//Update();
	}
}

void DeseaseSpreadSimulation::Simulation::Stop()
{
	stop = true;
}

void DeseaseSpreadSimulation::Simulation::Pause()
{
	pause = true;
	TimeManager::Instance().Pause();
}

void DeseaseSpreadSimulation::Simulation::Resume()
{
	pause = false;
	TimeManager::Instance().Start();
}

void DeseaseSpreadSimulation::Simulation::SetSimulationSpeedMultiplier(uint16_t multiplier)
{
	TimeManager::Instance().SetSimulationTimeMultiplier(multiplier);
}

/*
void DeseaseSpreadSimulation::Simulation::Update()
{
	TimeManager::Instance().Update();

	for (auto& community : communities)
	{
		for (auto& person : community.GetPopulation())
		{
			person->Update();
		}

		Contacts(community);
	}

	if (withPrint)
	{
		Print();
	}
}

void DeseaseSpreadSimulation::Simulation::Print()
{
}

void DeseaseSpreadSimulation::Simulation::Contacts(Community community)
{
	for (auto& place : community.GetPlaces())
	{
		auto people = place->GetPeople();

		// Get all susceptible and infectious people
		std::vector<Person*> susceptible;
		std::vector<Person*> infectious;
		for (auto person : people)
		{
			if (person->isSusceptible())
			{
				susceptible.push_back(person);
			}
			else if (person->isInfectious())
			{
				infectious.push_back(person);
			}
		}

		// Every infectious person has a chance to infect a susceptible person
		for (auto infectiousPerson : infectious)
		{
			for (auto susceptiblePerson : susceptible)
			{
				infectiousPerson->Contact(*susceptiblePerson);
			}
		}
	}
}

void DeseaseSpreadSimulation::Simulation::SetupEverything()
{
	CommunityBuilder cbuilder;
	communities.push_back(cbuilder.CreateCommunity(populationSize, country));

	TimeManager::Instance().Start();
	stop = false;
}
*/