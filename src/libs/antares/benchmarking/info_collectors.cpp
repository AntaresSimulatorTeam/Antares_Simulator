#include "info_collectors.h"
#include <antares/exception/LoadingError.hpp>
#include <antares/config.h>
#include "antares/study/area/area.h"

using namespace Antares::Data;

namespace Benchmarking 
{
	// Collecting data from study
	// ---------------------------
	void StudyInfoCollector::collect()
	{
		collectAreasCount();
		collectLinksCount();
		collectPerformedYearsCount();
		collectEnabledThermalClustersCount();
		collectEnabledBindingConstraintsCount();
		collectUnitCommitmentMode();
		collectMaxNbYearsInParallel();
        collectSolverVersion();
	}

	void StudyInfoCollector::collectAreasCount()
	{
		file_content_.addItemToSection("from study", "areas", study_.areas.size());
	}

	void StudyInfoCollector::collectLinksCount()
	{
		file_content_.addItemToSection("from study", "links", study_.areas.areaLinkCount());
	}

	void StudyInfoCollector::collectPerformedYearsCount()
	{
		// Computing the number of performed years
		unsigned int nbPerformedYears = 0;
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears++;
		}

		// Adding an item related to number of performed years to the file content
		file_content_.addItemToSection("from study", "performed years", nbPerformedYears);
	}

	void StudyInfoCollector::collectEnabledThermalClustersCount()
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
		file_content_.addItemToSection("from study", "enabled thermal clusters", nbEnabledThermalClusters);
	}

	void StudyInfoCollector::collectEnabledBindingConstraintsCount()
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

		file_content_.addItemToSection("from study", "enabled BC", nbEnabledBC);
		file_content_.addItemToSection("from study", "enabled hourly BC", nbEnabledHourlyBC);
		file_content_.addItemToSection("from study", "enabled daily BC", nbEnabledDailyBC);
		file_content_.addItemToSection("from study", "enabled weekly BC", nbEnabledWeeklyBC);
	}

	void StudyInfoCollector::collectUnitCommitmentMode()
	{
		const char* unitCommitment = UnitCommitmentModeToCString(study_.parameters.unitCommitment.ucMode);
		file_content_.addItemToSection("from study", "unit commitment", unitCommitment);
	}

	void StudyInfoCollector::collectMaxNbYearsInParallel()
	{
		file_content_.addItemToSection("from study", "max parallel years", study_.maxNbYearsInParallel);
	}

	void StudyInfoCollector::collectSolverVersion()
	{
		// Example : 8.3.0 -> 830
		const unsigned int version
			= 100 * ANTARES_VERSION_HI + 10 * ANTARES_VERSION_LO + ANTARES_VERSION_BUILD;

		file_content_.addItemToSection("from study", "antares version", version);
	}

	// Collecting data from simulation
	// ---------------------------------
	void SimulationInfoCollector::collect()
	{
		file_content_.addItemToSection("from simulation", "variables", opt_info_.nbVariables);
		file_content_.addItemToSection("from simulation", "constraints", opt_info_.nbConstraints);
		file_content_.addItemToSection("from simulation", "non-zero coefficients", opt_info_.nbNonZeroCoeffs);
	}


	// Collecting durations from simulation
	// -------------------------------------
	void DurationCollector::toFileContent(FileContent& file_content)
	{		
		for (pair<string, vector<int64_t>> element : duration_items_)
		{
			std::string name = element.first;
			vector<int64_t> durations = element.second;
			int64_t duration_sum = 0;
			for (auto& duration : durations)
			{
				duration_sum += duration;
			}

			file_content.addDurationItem(name, (unsigned int)duration_sum, (int)durations.size());
		}
	}

	void DurationCollector::addDuration(std::string name, int64_t duration)
	{	
		const std::lock_guard<std::mutex> lock(mutex_);
		duration_items_[name].push_back(duration);
	}

}
