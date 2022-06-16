#include "studyinfo.h"
#include <antares/exception/LoadingError.hpp>
#include "antares/study/area/area.h"
#include <algorithm>

using namespace Antares::Data;

namespace Benchmarking 
{
	StudyInfo::StudyInfo(const Antares::Data::Study& study) : study_(study)
	{}

	void StudyInfo::collect()
	{
		getAreasCount();
		getLinksCount();
		getPerformedYearsCount();
		getEnabledThermalClustersCount();
		getEnabledBindingConstraintsCount();
		getUnitCommitmentMode();
		getMaxNbYearsInParallel();
	}

	void StudyInfo::flush(Yuni::String& filePath)
	{
		if (!outputFile_.openRW(filePath))
		{
			throw Antares::Error::CreatingStudyInfoFile(filePath);
		}

		outputFile_ << "Number of areas" << "\t" << nbAreas_ << "\n";
		outputFile_ << "Number of links" << "\t" << nbLinks_ << "\n";
		outputFile_ << "Number of performed years" << "\t" << nbPerformedYears_ << "\n";
		outputFile_ << "Number of enabled thermal clusters" << "\t" << nbEnabledThermalClusters_ << "\n";
		outputFile_ << "Number of enabled BCs" << "\t" << nbEnabledBC_ << "\n";
		outputFile_ << "Number of enabled hourly BCs" << "\t" << nbEnabledHourlyBC_ << "\n";
		outputFile_ << "Number of enabled daily BCs" << "\t" << nbEnabledDailyBC_ << "\n";
		outputFile_ << "Number of enabled weekly BCs" << "\t" << nbEnabledWeeklyBC_ << "\n";
		outputFile_ << "Unit commitment mode" << "\t" << UnitComitmentMode_ << "\n";
		outputFile_ << "Max number of years in parallel" << "\t" << maxNbYearsInParallel_ << "\n";
	}

	void StudyInfo::getAreasCount()
	{
		nbAreas_ = study_.areas.size();
	}

	void StudyInfo::getLinksCount()
	{
		nbLinks_ = study_.areas.areaLinkCount();
	}

	void StudyInfo::getPerformedYearsCount()
	{
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears_++;
		}
	}

	void StudyInfo::getEnabledThermalClustersCount()
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

	void StudyInfo::getEnabledBindingConstraintsCount()
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

	void StudyInfo::getUnitCommitmentMode()
	{
		UnitComitmentMode_ = UnitCommitmentModeToCString(study_.parameters.unitCommitment.ucMode);
	}

	void StudyInfo::getMaxNbYearsInParallel()
	{
		maxNbYearsInParallel_ = study_.maxNbYearsInParallel;
	}
}
