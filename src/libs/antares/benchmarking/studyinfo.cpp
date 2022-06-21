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

	StudyInfoContainer::iterator StudyInfoContainer::begin()
	{
		return items_.begin();
	}

	StudyInfoContainer::iterator StudyInfoContainer::end()
	{
		return items_.end();
	}

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
		addItem<StudyInfoItem_integerValue>("areas", study_.areas.size());
	}

	void StudyInfoContainer::collectLinksCount()
	{
		addItem<StudyInfoItem_integerValue>("links", study_.areas.areaLinkCount());
	}

	void StudyInfoContainer::collectPerformedYearsCount()
	{
		// Computing the number of performed years
		unsigned int nbPerformedYears = 0;
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears++;
		}

		// Adding an item related to number of performed years to the file content
		addItem<StudyInfoItem_integerValue>("performed years", nbPerformedYears);
	}

	void StudyInfoContainer::collectEnabledThermalClustersCount()
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
				ThermalCluster* cluster = i->second.get();
				if (cluster->enabled)
					nbEnabledThermalClusters++;
			}
		}

		// Adding an item related to number of enabled thermal clusters to the file content
		addItem<StudyInfoItem_integerValue>("enabled thermal clusters", nbEnabledThermalClusters);
	}

	void StudyInfoContainer::collectEnabledBindingConstraintsCount()
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

		addItem<StudyInfoItem_integerValue>("enabled BC", nbEnabledBC);
		addItem<StudyInfoItem_integerValue>("enabled hourly BC", nbEnabledHourlyBC);
		addItem<StudyInfoItem_integerValue>("enabled daily BC", nbEnabledDailyBC);
		addItem<StudyInfoItem_integerValue>("enabled weekly BC", nbEnabledWeeklyBC);
	}

	void StudyInfoContainer::collectUnitCommitmentMode()
	{
		const char* unitCommitment = UnitCommitmentModeToCString(study_.parameters.unitCommitment.ucMode);
		addItem<StudyInfoItem_charValue>("unit commitment", unitCommitment);
	}

	void StudyInfoContainer::collectMaxNbYearsInParallel()
	{
		addItem<StudyInfoItem_integerValue>("max parallel years", study_.maxNbYearsInParallel);
	}


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

		StudyInfoContainer::iterator it = fileContent_.begin();
		for (; it != fileContent_.end(); it++)
			outputFile_ << (*it)->name() << " : " << (*it)->value() << "\n";
	}
}
