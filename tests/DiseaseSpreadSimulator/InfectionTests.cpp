#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include "Enums.h"
#include "Places/Community.h"
#include "Places/Places.h"
#include "Person/Person.h"
#include "Person/PersonBehavior.h"
#include "Disease/Disease.h"
#include "Disease/Infection.h"

namespace UnitTests
{
	// Don't warn on magic numbers for tests
	// NOLINTBEGIN(*-magic-numbers)
	class InfectionTest : public ::testing::Test
	{
	protected:
		// Disease
		std::string name = "a";
		std::pair<uint32_t, uint32_t> incubationPeriod{2U, 2U};
		uint32_t daysInfectious = 1;
		std::pair<uint32_t, uint32_t> diseaseDurationRange{3U, 3U};
		std::vector<float> mortalityByAge{0.F, 0.F, 0.F, 0.F, 0.F, 0.F, 0.F, 0.F, 0.F};
		std::pair<uint32_t, uint32_t> daysTillDeathRange{1U, 1U};
		std::pair<float, float> spreadFactor{1.F, 1.F};
		float testAccuracy{1.0F};
		std::pair<float, float> symptomsDevelopment{1.F, 1.F};
		DiseaseSpreadSimulation::Disease disease{name, incubationPeriod, daysInfectious, diseaseDurationRange, mortalityByAge, daysTillDeathRange, spreadFactor, testAccuracy, symptomsDevelopment};

		std::string deadlyName = "DeadlyTestDisease";
		std::pair<uint32_t, uint32_t> deadlyIncubationPeriod{1U, 1U};
		uint32_t deadlyDaysInfectious = 1;
		std::pair<uint32_t, uint32_t> deadlyDiseaseDurationRange{2U, 2U};
		std::vector<float> deadlyMortalityByAge{1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F};
		std::pair<uint32_t, uint32_t> deadlyDaysTillDeathRange{2U, 2U};
		std::pair<float, float> deadlySpreadFactor{1.F, 1.F};
		float deadlyTestAccuracy{1.0F};
		std::pair<float, float> deadlySymptomsDevelopment{1.F, 1.F};
		DiseaseSpreadSimulation::Disease deadlyDisease{deadlyName, deadlyIncubationPeriod, deadlyDaysInfectious, deadlyDiseaseDurationRange, deadlyMortalityByAge, deadlyDaysTillDeathRange, deadlySpreadFactor, deadlyTestAccuracy, deadlySymptomsDevelopment};

		// Age groups
		std::vector<DiseaseSpreadSimulation::Age_Group> ageGroups{DiseaseSpreadSimulation::Age_Group::UnderTen, DiseaseSpreadSimulation::Age_Group::UnderTwenty, DiseaseSpreadSimulation::Age_Group::UnderThirty, DiseaseSpreadSimulation::Age_Group::UnderFourty, DiseaseSpreadSimulation::Age_Group::UnderFifty, DiseaseSpreadSimulation::Age_Group::UnderSixty, DiseaseSpreadSimulation::Age_Group::UnderSeventy, DiseaseSpreadSimulation::Age_Group::UnderEighty, DiseaseSpreadSimulation::Age_Group::AboveEighty};

		// Person
		DiseaseSpreadSimulation::Home home{};
		DiseaseSpreadSimulation::PersonBehavior behavior{10U, 10U, 0.F, 0.F};
	};
	// NOLINTEND(*-magic-numbers)
	// No point in splitting the test
	// NOLINTNEXTLINE(*-cognitive-complexity)
	TEST_F(InfectionTest, Contaminate) // cppcheck-suppress syntaxError
	{
		for (auto ageGroup : ageGroups)
		{
			// Normal infection
			DiseaseSpreadSimulation::Infection infection;

			ASSERT_FALSE(infection.HasDisease());
			ASSERT_TRUE(infection.IsSusceptible());
			ASSERT_FALSE(infection.IsInfectious());

			infection.Contaminate(&disease, ageGroup);

			ASSERT_TRUE(infection.HasDisease());
			EXPECT_EQ(infection.GetDiseaseName(), name);
			EXPECT_TRUE(!infection.IsSusceptible());
			ASSERT_FALSE(infection.IsInfectious());
			EXPECT_FALSE(infection.IsFatal());

			// Deadly infection
			DiseaseSpreadSimulation::Infection deadlyInfection;

			ASSERT_FALSE(deadlyInfection.HasDisease());
			ASSERT_TRUE(deadlyInfection.IsSusceptible());
			ASSERT_FALSE(deadlyInfection.IsInfectious());

			deadlyInfection.Contaminate(&deadlyDisease, ageGroup);

			ASSERT_TRUE(deadlyInfection.HasDisease());
			EXPECT_EQ(deadlyInfection.GetDiseaseName(), deadlyName);
			EXPECT_TRUE(!deadlyInfection.IsSusceptible());
			EXPECT_TRUE(deadlyInfection.IsFatal());
		}
	}
	TEST_F(InfectionTest, UpdateWithoutDisease)
	{
		DiseaseSpreadSimulation::Person person(ageGroups.at(2), DiseaseSpreadSimulation::Sex::Female, behavior, nullptr, &home);

		DiseaseSpreadSimulation::Infection infection;

		ASSERT_FALSE(infection.HasDisease());
		ASSERT_TRUE(infection.IsSusceptible());
		ASSERT_FALSE(infection.IsInfectious());

		// Check that an update without a disease won't change anything
		static constexpr auto testSize{200U};
		for (auto i{0U}; i < testSize; i++)
		{
			infection.Update(person, true);
			ASSERT_TRUE(infection.IsSusceptible());
			ASSERT_FALSE(infection.IsInfectious());
		}
	}
	TEST_F(InfectionTest, UpdateWithDisease)
	{
		DiseaseSpreadSimulation::Person person(ageGroups.at(2), DiseaseSpreadSimulation::Sex::Female, behavior, nullptr, &home);

		DiseaseSpreadSimulation::Infection infection;

		ASSERT_FALSE(infection.HasDisease());
		ASSERT_TRUE(infection.IsSusceptible());
		ASSERT_FALSE(infection.IsInfectious());

		// Check update with disease
		infection.Contaminate(&disease, ageGroups.at(2));

		ASSERT_TRUE(infection.HasDisease());
		ASSERT_EQ(infection.GetDiseaseName(), name);
		ASSERT_FALSE(infection.IsSusceptible());
		ASSERT_FALSE(infection.IsInfectious());
		ASSERT_FALSE(infection.IsFatal());

		// Advance latent period
		infection.Update(person, true);
		EXPECT_FALSE(infection.IsInfectious());
		EXPECT_FALSE(infection.HasSymptoms());

		// Advance infectious period
		infection.Update(person, true);
		EXPECT_TRUE(infection.IsInfectious());
		EXPECT_TRUE(infection.HasSymptoms());
		EXPECT_FALSE(infection.HasRecovered());

		// Advance recovered period
		infection.Update(person, true);
		EXPECT_FALSE(infection.IsInfectious());
		EXPECT_TRUE(infection.HasSymptoms());
		EXPECT_FALSE(infection.HasRecovered());

		infection.Update(person, true);
		EXPECT_FALSE(infection.HasDisease());
		EXPECT_FALSE(infection.IsInfectious());
		EXPECT_FALSE(infection.HasSymptoms());
		EXPECT_TRUE(infection.HasRecovered());
	}
	TEST_F(InfectionTest, UpdateWithDeadlyDisease)
	{
		DiseaseSpreadSimulation::Person person(ageGroups.at(2), DiseaseSpreadSimulation::Sex::Female, behavior, nullptr, &home);

		ASSERT_TRUE(person.IsAlive());

		// Deadly infection
		DiseaseSpreadSimulation::Infection deadlyInfection;

		ASSERT_FALSE(deadlyInfection.HasDisease());
		ASSERT_TRUE(deadlyInfection.IsSusceptible());
		ASSERT_FALSE(deadlyInfection.IsInfectious());

		deadlyInfection.Contaminate(&deadlyDisease, ageGroups.at(2));

		ASSERT_TRUE(deadlyInfection.HasDisease());
		ASSERT_EQ(deadlyInfection.GetDiseaseName(), deadlyName);
		ASSERT_TRUE(!deadlyInfection.IsSusceptible());
		ASSERT_TRUE(deadlyInfection.IsFatal());

		deadlyInfection.Update(person, true);
		EXPECT_TRUE(deadlyInfection.IsInfectious());
		EXPECT_TRUE(deadlyInfection.HasSymptoms());
		EXPECT_FALSE(deadlyInfection.HasRecovered());
		EXPECT_TRUE(person.IsAlive());

		deadlyInfection.Update(person, true);
		EXPECT_FALSE(person.IsAlive());
	}
	TEST_F(InfectionTest, WillInfect)
	{
		// Community
		DiseaseSpreadSimulation::Community community(0U, DiseaseSpreadSimulation::Country::USA);
		DiseaseSpreadSimulation::Infection infection;
		infection.Contaminate(&disease, ageGroups.at(2));
		ASSERT_TRUE(infection.HasDisease());

		EXPECT_TRUE(infection.WillInfect(infection, 0.F, &community));

		// To negate the small chance of being infected that everyone has we check multiple times
		static constexpr uint32_t sampleSize{100};
		uint32_t willInfect = 0;

		for (size_t i = 0; i < sampleSize; i++)
		{
			if (infection.WillInfect(infection, 1.F, &community))
			{
				willInfect++;
			}
		}

		// Less than 20% should be infected
		EXPECT_LT(willInfect, sampleSize * 0.2F);
	}
} // namespace UnitTests
