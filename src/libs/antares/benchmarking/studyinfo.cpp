#include "studyinfo.h"
#include <antares/exception/LoadingError.hpp>
#include "antares/study/area/area.h"

using namespace Antares::Data;

namespace Benchmarking 
{
	StudyInfo::StudyInfo(const Antares::Data::Study& study) : study_(study)
	{}

	void StudyInfo::collect()
	{
		getNbPerformedYears();
		getEnabledThermalClusters();
	}

	void StudyInfo::flush(Yuni::String& filePath)
	{
		if (!outputFile_.openRW(filePath))
		{
			throw Antares::Error::CreatingStudyInfoFile(filePath);
		}

		outputFile_ << "Number of performed years" << "\t" << nbPerformedYears_ << "\n";
		outputFile_ << "Number of enabled thermal clusters" << "\t" << nbEnabledThermalClusters_ << "\n";
	}

	void StudyInfo::getNbPerformedYears()
	{
		for (uint i = 0; i < study_.parameters.nbYears; i++)
		{
			if (study_.parameters.yearsFilter[i])
				nbPerformedYears_++;
		}
	}

	void StudyInfo::getEnabledThermalClusters()
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
}
