#include "studyinfo.h"
#include <antares/exception/LoadingError.hpp>
#include "antares/study/area/area.h"
#include <algorithm>

using namespace Antares::Data;

namespace Benchmarking 
{
	/*
		=== class StudyInfoContainer ===
	*/
	StudyInfoContainer::StudyInfoContainer(const Antares::Data::Study& study) : study_(study)
	{}


	// Collecting data from study
	void StudyInfoContainer::collect()
	{
		collectAreasCount();
		collectLinksCount();
		collectPerformedYearsCount();
		collectEnabledThermalClustersCount();
		collectEnabledBindingConstraintsCount();
		collectUnitCommitmentMode();
		collectMaxNbYearsInParallel();
	}


	void StudyInfoContainer::collectAreasCount()
	{
		nbAreas_ = study_.areas.size();
	}

	void StudyInfoContainer::collectLinksCount()
	{
		nbLinks_ = study_.areas.areaLinkCount();
	}

	void StudyInfoContainer::collectPerformedYearsCount()
	{
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears_++;
		}
	}

	void StudyInfoContainer::collectEnabledThermalClustersCount()
	{
		auto end = study_.areas.end();
		for (auto i = study_.areas.begin(); i != end; ++i)
		{
			Area& area = *(i->second);
			auto end = area.thermal.list.end();
			for (auto i = area.thermal.list.begin(); i != end; ++i)
			{
				ThermalCluster* cluster = i->second.get();
				if (cluster->enabled)
					nbEnabledThermalClusters_++;
			}
		}
	}

	void StudyInfoContainer::collectEnabledBindingConstraintsCount()
	{
		nbEnabledBC_ = study_.runtime->bindingConstraintCount;
		for (uint i = 0; i < nbEnabledBC_; i++)
		{
			switch (study_.runtime->bindingConstraint[i].type)
			{
			case BindingConstraint::Type::typeHourly:
				nbEnabledHourlyBC_++;
				break;
			case BindingConstraint::Type::typeDaily:
				nbEnabledDailyBC_++;
				break;
			case BindingConstraint::Type::typeWeekly:
				nbEnabledWeeklyBC_++;
				break;
			default:
				break;
			}
		}
	}

	void StudyInfoContainer::collectUnitCommitmentMode()
	{
		UnitComitmentMode_ = UnitCommitmentModeToCString(study_.parameters.unitCommitment.ucMode);
	}

	void StudyInfoContainer::collectMaxNbYearsInParallel()
	{
		maxNbYearsInParallel_ = study_.maxNbYearsInParallel;
	}

	// Getters
	unsigned int StudyInfoContainer::getAreasCount() { return nbAreas_; }
	unsigned int StudyInfoContainer::getLinksCount() { return nbLinks_; }
	unsigned int StudyInfoContainer::getPerformedYearsCount() { return nbPerformedYears_; }
	unsigned int StudyInfoContainer::getEnabledThermalClustersCount() { return nbEnabledThermalClusters_; }
	const char* StudyInfoContainer::getUnitCommitmentMode() { return UnitComitmentMode_; }
	unsigned int StudyInfoContainer::getMaxNbYearsInParallel() { return maxNbYearsInParallel_; }

	unsigned int StudyInfoContainer::getEnabledBCcount() { return nbEnabledBC_; }
	unsigned int StudyInfoContainer::getEnabledHourlyBCcount() { return nbEnabledHourlyBC_; }
	unsigned int StudyInfoContainer::getEnabledDailyBCcount() { return nbEnabledDailyBC_; }
	unsigned int StudyInfoContainer::getEnabledWeeklyBCcount() { return nbEnabledWeeklyBC_; }


	/*
		=== class StudyInfoWriter ===
	*/

	StudyInfoWriter::StudyInfoWriter(Yuni::String& filePath, StudyInfoContainer& fileContent) 
		: filePath_(filePath), fileContent_(fileContent)
	{}

	void StudyInfoWriter::flush()
	{
		if (!outputFile_.openRW(filePath_))
		{
			throw Antares::Error::CreatingStudyInfoFile(filePath_);
		}

		outputFile_ << "Number of areas" << "\t" << fileContent_.getAreasCount() << "\n";
		outputFile_ << "Number of links" << "\t" << fileContent_.getLinksCount() << "\n";
		outputFile_ << "Number of performed years" << "\t" << fileContent_.getPerformedYearsCount() << "\n";
		outputFile_ << "Number of enabled thermal clusters" << "\t" << fileContent_.getEnabledThermalClustersCount() << "\n";

		outputFile_ << "Number of enabled BCs" << "\t" << fileContent_.getEnabledBCcount() << "\n";
		outputFile_ << "Number of enabled hourly BCs" << "\t" << fileContent_.getEnabledHourlyBCcount() << "\n";
		outputFile_ << "Number of enabled daily BCs" << "\t" << fileContent_.getEnabledDailyBCcount() << "\n";
		outputFile_ << "Number of enabled weekly BCs" << "\t" << fileContent_.getEnabledWeeklyBCcount() << "\n";

		outputFile_ << "Unit commitment mode" << "\t" << fileContent_.getUnitCommitmentMode() << "\n";
		outputFile_ << "Max number of years in parallel" << "\t" << fileContent_.getMaxNbYearsInParallel() << "\n";
		
		// Only for test : is about to be removed
		// std::string blabla("blabla");
		// outputFile_ << blabla;
	}
}
