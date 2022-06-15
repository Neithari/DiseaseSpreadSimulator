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
#include "Simulation/TimeManager.h"
#include "Disease/Disease.h"
#include "Disease/DiseaseContainment.h"


namespace UnitTests
{
	class DiseaseContainmentTest : public ::testing::Test
	{
	protected:
		// Person
		DiseaseSpreadSimulation::Home home{};
		DiseaseSpreadSimulation::PersonBehavior behavior{0u, 1u, 1.f, 0.f};
		DiseaseSpreadSimulation::Person person{DiseaseSpreadSimulation::Age_Group::UnderThirty, DiseaseSpreadSimulation::Sex::Male, behavior, &community, &home};

		// Community
		DiseaseSpreadSimulation::HardwareStore hwStore;
		DiseaseSpreadSimulation::Supply supplyStore;
		DiseaseSpreadSimulation::Morgue morgue;
		DiseaseSpreadSimulation::Workplace work;
		DiseaseSpreadSimulation::School school;
		DiseaseSpreadSimulation::Community community{0u, DiseaseSpreadSimulation::Country::USA};
		DiseaseSpreadSimulation::TimeManager time;
		// Values from Person.h
		static constexpr uint32_t shopOpenTime = 7u;
		static constexpr uint32_t shopCloseTime = 20u;
		static constexpr uint32_t workStartTime = 8u;
		static constexpr uint32_t workFinishTime = 17u;
		static constexpr uint32_t schoolStartTime = 8u;
		static constexpr uint32_t schoolFinishTime = 15u;

		void InitCommunity()
		{

			community.AddPlace(home);
			community.AddPlace(hwStore);
			community.AddPlace(supplyStore);
			community.AddPlace(morgue);
			community.AddPlace(work);
			community.AddPlace(school);
		}
	};
	TEST_F(DiseaseContainmentTest, Quarantine)
	{
		InitCommunity();

		DiseaseSpreadSimulation::DiseaseContainment containment;

		ASSERT_EQ(person.GetWhereabouts(), person.GetHome());
		ASSERT_FALSE(person.IsQuarantined());

		for (auto i = shopOpenTime; i <= shopCloseTime; i++)
		{
			person.Update(i, true, false);
			if (person.GetWhereabouts()->GetType() == DiseaseSpreadSimulation::Place_Type::Supply)
			{
				ASSERT_EQ(person.GetWhereabouts(), community.GetSupplyStore());
				containment.Quarantine(&person);
				break;
			}
		}
		EXPECT_EQ(person.GetWhereabouts(), person.GetHome());
		EXPECT_TRUE(person.IsQuarantined());

		// Advance day and check if the person will stay at home
		for (uint32_t day = 0; day < 10; day++)
		{
			person.Update(1, true, true);

			for (auto hour = shopOpenTime; hour <= shopCloseTime; hour++)
			{
				person.Update(hour, true, false);
				EXPECT_EQ(person.GetWhereabouts(), person.GetHome());
			}
		}
	}
	TEST_F(DiseaseContainmentTest, ReleaseWhenRecovered)
	{
		InitCommunity();
		// Disease
		std::string name = "a";
		std::pair<uint32_t, uint32_t> incubationPeriod{2u, 2u};
		uint32_t daysInfectious = 1;
		std::pair<uint32_t, uint32_t> diseaseDurationRange{3u, 3u};
		std::vector<float> mortalityByAge{0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		std::pair<uint32_t, uint32_t> daysTillDeathRange{1u, 1u};
		std::pair<float, float> spreadFactor{1.f, 1.f};
		float testAccuracy{1.0f};
		std::pair<float, float> symptomsDevelopment{1.f, 1.f};
		DiseaseSpreadSimulation::Disease disease{name, incubationPeriod, daysInfectious, diseaseDurationRange, mortalityByAge, daysTillDeathRange, spreadFactor, testAccuracy, symptomsDevelopment};

		DiseaseSpreadSimulation::DiseaseContainment containment;

		containment.Quarantine(&person);
		ASSERT_EQ(person.GetWhereabouts(), person.GetHome());
		ASSERT_TRUE(person.IsQuarantined());

		person.Contaminate(&disease);
		do
		{
			person.Update(1, false, true);
		} while (!person.HasRecovered());

		containment.ReleaseWhenRecovered(&person);
		EXPECT_FALSE(person.IsQuarantined());

		for (auto i = shopOpenTime; i <= shopCloseTime; i++)
		{
			person.Update(i, true, false);
			if (person.GetWhereabouts()->GetType() == DiseaseSpreadSimulation::Place_Type::Supply)
			{
				EXPECT_EQ(person.GetWhereabouts(), community.GetSupplyStore());
				break;
			}
		}
	}
} // namespace UnitTests
