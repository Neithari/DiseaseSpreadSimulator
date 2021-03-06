#pragma once
#include <string>
#include <vector>
#include <utility>
#include "nlohmann/json.hpp"
#include "Enums.h"

namespace DiseaseSpreadSimulation
{
	class Disease
	{
	public:
		Disease(std::string name,
			std::pair<uint32_t, uint32_t> incubationPeriod,
			uint32_t daysInfectious,
			std::pair<uint32_t, uint32_t> diseaseDurationRange,
			std::vector<float> mortalityByAge,
			std::pair<uint32_t, uint32_t> daysTillDeathRange,
			std::pair<float, float> spreadFactor = {1.0F, 1.0F},
			float testAccuracy = 1.0F,
			std::pair<float, float> symptomsDevelopment = {1.0F, 1.0F});

	public:
		// Check with ID
		inline bool operator==(const Disease& rhs) const
		{
			return m_id == rhs.m_id;
		};

		[[nodiscard]] const std::string& GetDiseaseName() const;
		[[nodiscard]] uint32_t IncubationPeriod() const;
		[[nodiscard]] uint32_t DaysInfectious() const;
		[[nodiscard]] float GetMortalityByAge(uint32_t age) const;
		[[nodiscard]] float GetMortalityByAgeGroup(Age_Group age) const;
		// Return a random duration out of the range
		[[nodiscard]] uint32_t GetDiseaseDuration() const;
		// Return a random death time out of the range
		[[nodiscard]] uint32_t DaysTillDeath() const;
		[[nodiscard]] float GetSpreadFactor() const;
		[[nodiscard]] float GetTestAccuracy() const;

		[[nodiscard]] uint32_t GetID() const;
		// Check without ID
		[[nodiscard]] bool isSame(const Disease& other) const;
		[[nodiscard]] bool hasSameID(const Disease& other) const;
		[[nodiscard]] bool isFatal(Age_Group age) const;
		[[nodiscard]] bool willDevelopSymptoms() const;

		// For nlohmann/json conversion
		friend struct nlohmann::adl_serializer<Disease, void>;

	private:
		const uint32_t m_id{0};
		const std::string m_name{};
		// Incubation period in days
		const std::pair<uint32_t, uint32_t> m_incubationPeriod{};
		// Patient is contagious for x days after start of symptoms
		const uint32_t m_daysInfectious{0};
		// Duration min, max in days
		const std::pair<uint32_t, uint32_t> m_durationRange{};
		// Mortality in percent from 0-1, age from 0-9, 10-19, 20-29,...., >80 years
		const std::vector<float> m_mortalityByAge{};
		const std::pair<uint32_t, uint32_t> m_daysTillDeathRange{};
		// In percent from 0-1 how likely it is to get infected at exposure
		const std::pair<float, float> m_spreadFactor{};
		const float m_testAccuracy{1.0F};
		// Chance that symptoms will be developed, in percent from 0-1
		const std::pair<float, float> m_symptomsDevelopment{};
	};
} // namespace DiseaseSpreadSimulation

// For nlohmann/json conversion
namespace nlohmann
{
	template <>
	struct adl_serializer<DiseaseSpreadSimulation::Disease>
	{
		static DiseaseSpreadSimulation::Disease from_json(const json& j)
		{
			return {
				j["Name"].get<std::string>(),
				j["Incubation Period"].get<std::pair<uint32_t, uint32_t>>(),
				j["Days Infectious"].get<uint32_t>(),
				j["Disease Duration Range"].get<std::pair<uint32_t, uint32_t>>(),
				j["Mortality By Age"].get<std::vector<float>>(),
				j["Days Till Death Range"].get<std::pair<uint32_t, uint32_t>>(),
				j["Spread Factor"].get<std::pair<float, float>>(),
				j["Test Accuracy"].get<float>(),
				j["Symptoms Development"].get<std::pair<float, float>>()};
		}

		static void to_json(json& j, const DiseaseSpreadSimulation::Disease& disease)
		{
			j = {
				{"Name", disease.m_name},
				{"Incubation Period", disease.m_incubationPeriod},
				{"Days Infectious", disease.m_daysInfectious},
				{"Disease Duration Range", disease.m_durationRange},
				{"Mortality By Age", disease.m_mortalityByAge},
				{"Days Till Death Range", disease.m_daysTillDeathRange},
				{"Spread Factor", disease.m_spreadFactor},
				{"Test Accuracy", disease.m_testAccuracy},
				{"Symptoms Development", disease.m_symptomsDevelopment}};
		}
	};
} // namespace nlohmann
