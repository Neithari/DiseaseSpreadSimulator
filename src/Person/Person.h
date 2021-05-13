#pragma once
#include "Places/Places.h"

namespace DeseaseSpreadSimulation
{
	class Person
	{
	public:
		// Create a Person with age, sex, set it's home and set whereabout to home
		Person(Age_Group age, Sex sex, std::shared_ptr<Home> home);

		void Update();

		void Contact(Person& other);
		std::string GetDeseaseName() const;
		void Contaminate(const Desease* infection);
		// Advance daysTillOutbreak, daysContagious, daysTillCured, daysToLive by a delta time
		void AdvanceDay();

		bool isSusceptible() const;
		bool isInfectious() const;
		bool isQuarantined() const;
		bool isAlive() const;
		bool hasDesease(const std::string& deseaseName) const;

		Age_Group GetAgeGroup() const;
		Sex GetSex() const;
		auto GetWhereabouts() const;
		auto GetHome() const;

	private:
		void Move();
		void DeseaseCheck();

	private:
		Age_Group age;
		Sex sex;
		std::shared_ptr<Home> home;
		std::shared_ptr<Place> whereabouts;
		std::shared_ptr<Place> workplace;

		Seir_State state = Seir_State::Susceptible;
		bool quarantined = false;
		unsigned int spreadCount = 0;

		// Desease Stuff
		const Desease* desease = nullptr;
		unsigned int latentPeriod = 0;
		unsigned int daysInfectious = 0;
		unsigned int daysTillCured = 0;

		bool alive = true;
		unsigned int daysToLive = 0;
		bool willDie = false;
	};
}
