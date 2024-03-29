#include "Disease/DiseaseBuilder.h"
#include <fstream>
#include <iostream>

DiseaseSpreadSimulation::Disease DiseaseSpreadSimulation::DiseaseBuilder::CreateCorona()
{
	// https://www.rki.de/DE/Content/InfAZ/N/Neuartiges_Coronavirus/Steckbrief.html
	// https://elemental.medium.com/from-infection-to-recovery-how-long-it-lasts-199e266fd018
	SetDiseaseName("COVID-19");
	static constexpr uint32_t coronaIncubationPeriodMin{1};
	static constexpr uint32_t coronaIncubationPeriodMax{14};
	SetIncubationPeriod(coronaIncubationPeriodMin, coronaIncubationPeriodMax);
	static constexpr uint32_t coronaDaysInfectious{10};
	SetDaysInfectious(coronaDaysInfectious);
	static constexpr uint32_t coronaDurationMin{14};
	static constexpr uint32_t coronaDurationMax{56};
	SetDiseaseDuration(coronaIncubationPeriodMin + coronaDurationMin, coronaIncubationPeriodMax + coronaDurationMax);
	static const std::vector<float> coronaMortalityByAge{0.0F, 0.0014F, 0.0012F, 0.002F, 0.0038F, 0.0098F, 0.0298F, 0.0794F, 0.1734F};
	SetMortalityByAge(coronaMortalityByAge);
	SetDaysTillDeath(coronaDurationMin, coronaDurationMax);

	// According to multiple sources only few individuals infect a lot of people.
	// We use a log normal distribution with most people in the low percents because of that.
	static constexpr float coronaSpreadFactorMin{0.0F};
	static constexpr float coronaSpreadFactorMax{0.5F};
	SetSpreadFactor(coronaSpreadFactorMin, coronaSpreadFactorMax);
	static constexpr float coronaTestAccuracy{0.981F};
	SetTestAccuracy(coronaTestAccuracy);
	// https://www.rki.de/DE/Content/InfAZ/N/Neuartiges_Coronavirus/Steckbrief.html -> Manifestationsindex
	static constexpr float coronaSymptomsDevelopmentMin{0.55F};
	static constexpr float coronaSymptomsDevelopmentMax{0.85F};
	SetSymptomsDevelopment(coronaSymptomsDevelopmentMin, coronaSymptomsDevelopmentMax);

	return CreateDisease();
}

DiseaseSpreadSimulation::Disease DiseaseSpreadSimulation::DiseaseBuilder::CreateDeadlyTestDisease()
{
	SetDiseaseName("DeadlyTestDisease");
	static constexpr uint32_t deadlyIncubationPeriodMin{1};
	static constexpr uint32_t deadlyIncubationPeriodMax{1};
	SetIncubationPeriod(deadlyIncubationPeriodMin, deadlyIncubationPeriodMax);
	static constexpr uint32_t deadlyDaysInfectious{10};
	SetDaysInfectious(deadlyDaysInfectious);
	static constexpr uint32_t deadlyDurationMin{10};
	static constexpr uint32_t deadlyDurationMax{10};
	SetDiseaseDuration(deadlyDurationMin, deadlyDurationMax);
	static const std::vector<float> deadlyMortalityByAge{1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F};
	SetMortalityByAge(deadlyMortalityByAge);
	SetDaysTillDeath(deadlyDurationMin, deadlyDurationMax);
	static constexpr float deadlySpreadFactorMin{1.F};
	static constexpr float deadlySpreadFactorMax{1.F};
	SetSpreadFactor(deadlySpreadFactorMin, deadlySpreadFactorMax);
	static constexpr float deadlyTestAccuracy{1.F};
	SetTestAccuracy(deadlyTestAccuracy);
	static constexpr float deadlySymptomsDevelopmentMin{1.F};
	static constexpr float deadlySymptomsDevelopmentMax{1.F};
	SetSymptomsDevelopment(deadlySymptomsDevelopmentMin, deadlySymptomsDevelopmentMax);

	return CreateDisease();
}

// Silence false positive because we move the name
// NOLINTNEXTLINE(performance-unnecessary-value-param)
void DiseaseSpreadSimulation::DiseaseBuilder::SetDiseaseName(std::string diseaseName)
{
	setupDone[0] = true;

	name = std::move(diseaseName);
}

// Silence clang tidy a std::pair would be less readable
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void DiseaseSpreadSimulation::DiseaseBuilder::SetIncubationPeriod(const uint32_t minDays, const uint32_t maxDays)
{
	setupDone[1] = true;

	incubationPeriod = {minDays, maxDays};
}

void DiseaseSpreadSimulation::DiseaseBuilder::SetDaysInfectious(const uint32_t days)
{
	setupDone[2] = true;

	daysInfectious = days;
}

// Silence clang tidy a std::pair would be less readable
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void DiseaseSpreadSimulation::DiseaseBuilder::SetDiseaseDuration(const uint32_t minDays, const uint32_t maxDays)
{
	setupDone[3] = true;

	diseaseDurationRange = {minDays, maxDays};
}

void DiseaseSpreadSimulation::DiseaseBuilder::SetMortalityByAge(std::vector<float> mortality)
{
	setupDone[4] = true;

	mortalityByAge = std::move(mortality);
}

// Silence clang tidy a std::pair would be less readable
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void DiseaseSpreadSimulation::DiseaseBuilder::SetDaysTillDeath(const uint32_t min, const uint32_t max)
{
	setupDone[5] = true; // NOLINT: Silence magic number false positive

	daysTillDeathRange = {min, max};
}

// Silence clang tidy a std::pair would be less readable
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void DiseaseSpreadSimulation::DiseaseBuilder::SetSpreadFactor(const float minFactor, const float maxFactor)
{
	setupDone[6] = true; // NOLINT: Silence magic number false positive

	spreadFactor = {minFactor, maxFactor};
}

void DiseaseSpreadSimulation::DiseaseBuilder::SetTestAccuracy(const float accuracy)
{
	setupDone[7] = true; // NOLINT: Silence magic number false positive

	testAccuracy = accuracy;
}

// Silence clang tidy a std::pair would be less readable
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void DiseaseSpreadSimulation::DiseaseBuilder::SetSymptomsDevelopment(const float minPercent, const float maxPercent)
{
	setupDone[8] = true; // NOLINT: Silence magic number false positive

	symptomsDevelopment = {minPercent, maxPercent};
}

DiseaseSpreadSimulation::Disease DiseaseSpreadSimulation::DiseaseBuilder::CreateDisease()
{
	// Will throw if you didn't setup everything befor trying to create the disease
	if (!std::all_of(setupDone.begin(), setupDone.end(), [](bool step)
			{
				return step;
			}))
	{
		throw std::logic_error("Complete setup to create a disease!");
	}

	return {name,
		incubationPeriod,
		daysInfectious,
		diseaseDurationRange,
		mortalityByAge,
		daysTillDeathRange,
		spreadFactor,
		testAccuracy,
		symptomsDevelopment};
}

std::vector<DiseaseSpreadSimulation::Disease> DiseaseSpreadSimulation::DiseaseBuilder::CreateDiseasesFromFile(const std::string& filename)
{
	using json = nlohmann::json;
	std::ifstream diseaseJsonFile{filename};

	if (!diseaseJsonFile)
	{

		std::cerr << filename << " could not be opened for reading!\n";
		return {};
	}

	json diseaseJson; // NOLINT: The library is already initializing it to null
	diseaseJsonFile >> diseaseJson;

	std::vector<Disease> diseases{diseaseJson.begin(), diseaseJson.end()};

	return diseases;
}

void DiseaseSpreadSimulation::DiseaseBuilder::SaveDiseaseToFile(const std::string& diseaseSaveName, const Disease& disease, const std::string& filename)
{
	using json = nlohmann::json;

	// Because of the json formatting we can't just append the new desease
	json diseaseJson; // NOLINT: The library is already initializing it to null

	// We need to check if the file exists
	std::ifstream fileExists{filename};
	json existingDiseaseJson; // NOLINT: The library is already initializing it to null
	if (fileExists)
	{
		// Copy all contents
		fileExists >> existingDiseaseJson;
		// Append them
		for (auto& [key, existingDisease] : existingDiseaseJson.items())
		{
			diseaseJson.emplace(key, existingDisease);
		}
	}
	fileExists.close();

	// Append the new desease
	diseaseJson.emplace(diseaseSaveName, disease);

	std::ofstream diseaseSaveFile{filename};

	if (!diseaseSaveFile)
	{
		std::cerr << filename << " could not be opened for writing!\n";
		return;
	}

	// Write it
	diseaseSaveFile << std::setw(4) << diseaseJson << std::endl;
}
