#pragma once

namespace DiseaseSpreadSimulation
{
	class Disease
	{
	public:
		Disease(std::string name,
			const std::pair<uint16_t, uint16_t> incubationPeriod,
			const uint16_t daysInfectious,
			std::pair<uint16_t, uint16_t> diseaseDurationRange,
			std::vector<float> mortalityByAge,
			std::pair<uint16_t, uint16_t> daysTillDeathRange,
			std::pair<float, float> spreadFactor = { 1.0f, 1.0f },
			float testAccuracy = 1.0f,
			std::pair<float, float> symptomsDevelopment = { 1.0f, 1.0f });

		// Check with ID
		inline bool operator==(const Disease& rhs) const
		{
			return	id == rhs.id;
		};

		const std::string& GetDiseaseName() const;
		uint16_t IncubationPeriod() const;
		uint16_t DaysInfectious() const;
		float GetMortalityByAge(uint16_t age) const;
		float GetMortalityByAgeGroup(Age_Group age) const;
		// Return a random duration out of the range
		uint16_t GetDiseaseDuration() const;
		// Return a random death time out of the range
		uint16_t DaysTillDeath() const;
		float GetSpreadFactor() const;
		float GetTestAccuracy() const;

		uint32_t GetID() const;
		// Check without ID
		bool isSame(const Disease& other) const;
		bool hasSameID(const Disease& other) const;
		bool isFatal(Age_Group age) const;
		bool willDevelopSymptoms() const;

	private:
		const uint32_t id = 0;
		const std::string name{};
		// Incubation period in days
		const std::pair<uint16_t, uint16_t> incubationPeriod{};
		// Patient is contagious for x days after start of symptoms
		const uint16_t daysInfectious = 0;
		// Duration min, max in days
		const std::pair<uint16_t, uint16_t> durationRange{};
		// Mortality in percent from 0-1, age from 0-9, 10-19, 20-29,...., >80 years
		const std::vector<float> mortalityByAge;
		const std::pair<uint16_t, uint16_t> daysTillDeathRange{};
		// In percent from 0-1 how likely it is to get infected at exposure
		std::pair<float, float> spreadFactor{};
		float testAccuracy = 1.0f;
		std::pair<float, float> symptomsDevelopment{};
	};
}