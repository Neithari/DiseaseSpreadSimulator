#include "pch.h"
#include "Person/Person.h"
#include "IDGenerator/IDGenerator.h"

DeseaseSpreadSimulation::Person::Person(Age_Group age, Sex sex, Home* home)
	:
	id(IDGenerator::IDGenerator<Person>::GetNextID()),
	age(age),
	sex(sex),
	home(home),
	whereabouts(home)
{
}

void DeseaseSpreadSimulation::Person::Update()
{
	// move person
	Move();
	//  if we have a desease...
	if (desease != nullptr)
	{
		// ...advance day
		AdvanceDay();
		// ...desease check
		DeseaseCheck();
	}
}

void DeseaseSpreadSimulation::Person::Contact(Person& other)
{
	/// TODO: Should be possible to only alter myself and take other as const

	// if the other person is infectious and I have no desease, now I have
	if (other.isInfectious() && isSusceptible())
	{
		Contaminate(other.desease);
		other.spreadCount++;
	}
	// if I am infectious and the other person has no desease, now he has
	else if (isInfectious() && other.isSusceptible())
	{
		other.Contaminate(desease);
		spreadCount++;
	}
}

std::string DeseaseSpreadSimulation::Person::GetDeseaseName() const
{
	if (desease != nullptr)
	{
		return desease->GetDeseaseName();
	}
	return "";
}

void DeseaseSpreadSimulation::Person::Contaminate(const Desease* infection)
{
	desease = infection;
	state = Seir_State::Exposed;
	
	latentPeriod = desease->IncubationPeriod();
	daysInfectious = desease->DaysInfectious();
	daysTillCured = desease->GetDeseaseDuration();
	// check if the person will die from the infection
	if (desease->isFatal(age))
	{
		willDie = true;
		daysToLive = desease->DaysTillDeath();
	}
}

bool DeseaseSpreadSimulation::Person::isSusceptible() const
{
	return state == Seir_State::Susceptible;
}

bool DeseaseSpreadSimulation::Person::isInfectious() const
{
	return state == Seir_State::Infectious;
}

bool DeseaseSpreadSimulation::Person::isQuarantined() const
{
	return quarantined;
}

bool DeseaseSpreadSimulation::Person::isAlive() const
{
	return alive;
}

bool DeseaseSpreadSimulation::Person::hasDesease(const std::string& deseaseName) const
{
	if (desease == nullptr)
	{
		return false;
	}
	return desease->GetDeseaseName() == deseaseName;
	
}

uint32_t DeseaseSpreadSimulation::Person::GetID() const
{
	return id;
}

DeseaseSpreadSimulation::Age_Group DeseaseSpreadSimulation::Person::GetAgeGroup() const
{
	return age;
}

DeseaseSpreadSimulation::Sex DeseaseSpreadSimulation::Person::GetSex() const
{
	return sex;
}

auto DeseaseSpreadSimulation::Person::GetWhereabouts() const
{
	return whereabouts;
}

auto DeseaseSpreadSimulation::Person::GetHome() const
{
	return home;
}

void DeseaseSpreadSimulation::Person::DeseaseCheck()
{
	// A person is infectious when it was exposed to a desease and 
	if (state == Seir_State::Exposed && latentPeriod <= 0)
	{
		state = Seir_State::Infectious;
	}
	// it is recovered when it is 
	else if (state == Seir_State::Infectious && daysInfectious <= 0)
	{
		state = Seir_State::Recovered;
	}
	else if (state == Seir_State::Recovered && daysTillCured == 0)
	{
		desease = nullptr;
	}
}

void DeseaseSpreadSimulation::Person::SetWorkplace(Place* newWorkplace)
{
	workplace = newWorkplace;
}

void DeseaseSpreadSimulation::Person::Move()
{
	/// TODO: implement move function
}

void DeseaseSpreadSimulation::Person::AdvanceDay()
{
	// If the person has no desease, has recovered, is immune or dead do nothing (recovered/immune/dead are all Seir_State::Recovered)
	if (state == Seir_State::Susceptible || state == Seir_State::Recovered)
	{
		return;
	}
	if (latentPeriod > 0)
	{
		latentPeriod--;
	}
	if (daysInfectious > 0)
	{
		daysInfectious--;
	}
	if (daysTillCured > 0)
	{
		daysTillCured--;
	}
	if (willDie)
	{
		// decrement daysToLive and if it reached 0 the person will die
		if (--daysToLive <= 0)
		{
			alive = false;
		}
	}
}
