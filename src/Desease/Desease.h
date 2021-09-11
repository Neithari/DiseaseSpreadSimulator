#pragma once

namespace DeseaseSpreadSimulation
{
	enum class Seir_State { Susceptible, Exposed, Infectious, Recovered };
	// Age groups to differentiate between parts of the population from 0-9 years as first group up to > than 80 years as last group
	enum class Age_Group { UnderTen, UnderTwenty, UnderThirty, UnderFourty, UnderFifty, UnderSixty, UnderSeventy, UnderEighty, AboveEighty };

	enum class Sex { Female, Male };

	class Desease
	{
	public:
		Desease(std::string name, const int incubationPeriod, const int daysInfectious, std::pair<int, int> deseaseDurationRange, std::vector<float> mortalityByAge, std::pair<int, int> daysTillDeathRange);

		// Check with ID
		inline bool operator==(const Desease& rhs) const
		{
			return	id == rhs.id;
		};

		const std::string& GetDeseaseName() const;
		int IncubationPeriod() const;
		int DaysInfectious() const;
		float GetMortalityByAge(int age) const;
		float GetMortalityByAgeGroup(Age_Group age) const;
		int GetDeseaseDuration() const;
		int DaysTillDeath() const;

		uint32_t GetID() const;
		// Check without ID
		bool isSame(const Desease& other) const;
		bool hasSameID(const Desease& other) const;
		bool isFatal(Age_Group age) const;

	private:
		const uint32_t id = 0;
		const std::string name{};
		// incubation period in days
		const int incubationPeriod = 0;
		// patient is contagious for x days after start of symptoms
		const int daysInfectious = 0;
		// duration min, max in days
		const std::pair<int, int> durationRange{};
		// mortality in percent from 0-1, age from 0-9, 10-19, 20-29,...., >80 years
		const std::vector<float> mortalityByAge;
		const std::pair<int, int> daysTillDeathRange{};
	};
}
