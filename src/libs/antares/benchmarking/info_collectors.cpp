#include <numeric>
#include "info_collectors.h"
#include <antares/config.h>
#include <antares/Enum.hpp>
#include <antares/study/area.h>

using namespace Antares::Data;

namespace Benchmarking 
{
	// Collecting data study
	// ---------------------------
	void StudyInfoCollector::toFileContent(FileContent& file_content)
	{
		areasCountToFileContent(file_content);
		linksCountToFileContent(file_content);
		performedYearsCountToFileContent(file_content);
		enabledThermalClustersCountToFileContent(file_content);
		enabledBindingConstraintsCountToFileContent(file_content);
		unitCommitmentModeToFileContent(file_content);
		maxNbYearsInParallelToFileContent(file_content);
        solverVersionToFileContent(file_content);
        ORToolsUsed(file_content);
        ORToolsSolver(file_content);
	}

	void StudyInfoCollector::areasCountToFileContent(FileContent& file_content)
	{
		file_content.addItemToSection("study", "areas", study_.areas.size());
	}

	void StudyInfoCollector::linksCountToFileContent(FileContent& file_content)
	{
		file_content.addItemToSection("study", "links", study_.areas.areaLinkCount());
	}

	void StudyInfoCollector::performedYearsCountToFileContent(FileContent& file_content)
	{
		// Computing the number of performed years
		unsigned int nbPerformedYears = 0;
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears++;
		}

		// Adding an item related to number of performed years to the file content
		file_content.addItemToSection("study", "performed years", nbPerformedYears);
	}

	void StudyInfoCollector::enabledThermalClustersCountToFileContent(FileContent& file_content)
	{
		// Computing the number of enabled thermal clusters
		unsigned int nbEnabledThermalClusters = 0;

		auto end = study_.areas.end();
		for (auto i = study_.areas.begin(); i != end; ++i)
		{
			Area& area = *(i->second);
			auto end = area.thermal.list.end();
			for (auto i = area.thermal.list.begin(); i != end; ++i)
			{
				auto& cluster = i->second;
				if (cluster->enabled)
					nbEnabledThermalClusters++;
			}
		}

		// Adding an item related to number of enabled thermal clusters to the file content
		file_content.addItemToSection("study", "enabled thermal clusters", nbEnabledThermalClusters);
	}

	void StudyInfoCollector::enabledBindingConstraintsCountToFileContent(FileContent& file_content)
	{
		unsigned int nbEnabledBC = study_.runtime->bindingConstraintCount;
		unsigned int nbEnabledHourlyBC(0), nbEnabledDailyBC(0), nbEnabledWeeklyBC(0);

		for (uint i = 0; i < nbEnabledBC; i++)
		{
			switch (study_.runtime->bindingConstraint[i].type)
			{
			case BindingConstraint::Type::typeHourly:
				nbEnabledHourlyBC++;
				break;
			case BindingConstraint::Type::typeDaily:
				nbEnabledDailyBC++;
				break;
			case BindingConstraint::Type::typeWeekly:
				nbEnabledWeeklyBC++;
				break;
			default:
				break;
			}
		}

		file_content.addItemToSection("study", "enabled BC", nbEnabledBC);
		file_content.addItemToSection("study", "enabled hourly BC", nbEnabledHourlyBC);
		file_content.addItemToSection("study", "enabled daily BC", nbEnabledDailyBC);
		file_content.addItemToSection("study", "enabled weekly BC", nbEnabledWeeklyBC);
	}

	void StudyInfoCollector::unitCommitmentModeToFileContent(FileContent& file_content)
	{
		const char* unitCommitment = UnitCommitmentModeToCString(study_.parameters.unitCommitment.ucMode);
		file_content.addItemToSection("study", "unit commitment", unitCommitment);
	}

	void StudyInfoCollector::maxNbYearsInParallelToFileContent(FileContent& file_content)
	{
		file_content.addItemToSection("study", "max parallel years", study_.maxNbYearsInParallel);
	}

	void StudyInfoCollector::solverVersionToFileContent(FileContent& file_content)
	{
		// Example : 8.3.0 -> 830
		const unsigned int version
			= 100 * ANTARES_VERSION_HI + 10 * ANTARES_VERSION_LO + ANTARES_VERSION_BUILD;

		file_content.addItemToSection("study", "antares version", version);
	}

    void StudyInfoCollector::ORToolsUsed(FileContent& file_content)
    {
        const bool& ortoolsUsed = study_.parameters.ortoolsUsed;
        file_content.addItemToSection("study", "ortools used", ortoolsUsed ? "true" : "false");
    }

    void StudyInfoCollector::ORToolsSolver(FileContent& file_content)
    {
        const bool& ortoolsUsed = study_.parameters.ortoolsUsed;
        std::string ortoolsSolver;
        if (ortoolsUsed)
        {
            ortoolsSolver = Enum::toString<OrtoolsSolver>(study_.parameters.ortoolsEnumUsed);
        }
        else
        {
             ortoolsSolver = "none";
        }
        file_content.addItemToSection("study", "ortools solver", ortoolsSolver);
    }

	// Collecting data optimization problem
	// -------------------------------------
	void SimulationInfoCollector::toFileContent(FileContent& file_content)
	{
		file_content.addItemToSection("optimization problem", "variables", opt_info_.nbVariables);
		file_content.addItemToSection("optimization problem", "constraints", opt_info_.nbConstraints);
		file_content.addItemToSection("optimization problem", "non-zero coefficients", opt_info_.nbNonZeroCoeffs);
	}

	// Collecting durations from simulation
	// -------------------------------------
	void DurationCollector::toFileContent(FileContent& file_content)
	{		
		for (pair<string, vector<int64_t>> element : duration_items_)
		{
			const std::string& name = element.first;
			const auto& durations = element.second;
			const int64_t duration_sum = accumulate(durations.begin(), durations.end(), (int64_t)0);

			file_content.addDurationItem(name, (unsigned int)duration_sum, (int)durations.size());
		}
	}

	void DurationCollector::addDuration(const std::string& name, int64_t duration)
	{	
		const std::lock_guard<std::mutex> lock(mutex_);
		duration_items_[name].push_back(duration);
	}

}
