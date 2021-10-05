#define BOOST_TEST_MODULE test solver simulation things

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <study.h>
#include <timeseries-numbers.h>

using namespace Yuni;
using namespace Antares::Data;
using namespace Antares::Solver::TimeSeriesNumbers;


void initializeStudy(Study::Ptr study)
{
	study->parameters.derated = false;

	study->runtime = new StudyRuntimeInfos(1);
	study->runtime->rangeLimits.year[rangeBegin] = 0;
	study->runtime->rangeLimits.year[rangeEnd] = 0;
	study->runtime->rangeLimits.year[rangeCount] = 1;

	study->parameters.intraModal = 0;
	study->parameters.interModal = 0;
	study->parameters.timeSeriesToRefresh = 0;
}

// ========================
// Add an area to study
// ========================
Area* addAreaToStudy(Study::Ptr study, const std::string& areaName)
{
	Area* area = study->areaAdd(areaName);
	BOOST_CHECK(area != NULL);

	return area;
}

// ===========================
// Add a cluster to an area
// ===========================
template<class ClusterType>
void addClusterToAreaList(Area* area, std::shared_ptr<ClusterType> T)
{}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
	auto addedCluster = area->thermal.list.add(cluster);
	area->thermal.list.mapping[cluster->id()] = addedCluster;
}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
	area->renewable.list.add(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
	auto cluster = std::make_shared<ClusterType>(area);
	cluster->setName(clusterName);

	// cluster->series->series.resize(nbTS, HOURS_PER_YEAR);
	addClusterToAreaList(area, cluster);

	return cluster;
}


BOOST_AUTO_TEST_CASE(two_areas_with_5_ready_made_ts_on_load___intra_modal_ok)
{
	// Creating a study
	Study::Ptr study = new Study();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesLoad;

	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->resizeAllTimeseriesNumbers(1);
	area_1->load.series->series.resize(5, 1);

	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->resizeAllTimeseriesNumbers(1);
	area_2->load.series->series.resize(5, 1);

	BOOST_CHECK(Generate(*study));
}

BOOST_AUTO_TEST_CASE(two_areas_with_respectively_5_and_4_ready_made_ts_on_load___intra_modal_ko)
{
	// Creating a study
	Study::Ptr study = new Study();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesLoad;

	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->resizeAllTimeseriesNumbers(1);
	area_1->load.series->series.resize(5, 1);

	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->resizeAllTimeseriesNumbers(1);
	area_2->load.series->series.resize(4, 1);

	BOOST_CHECK(not Generate(*study));
}


/*
BOOST_AUTO_TEST_CASE(check_intra_modal_for_thermal)
{
	// Creating a study
	Study::Ptr study = new Study();

	study->parameters.intraModal |= timeSeriesThermal;

	Area* area_1 = addAreaToStudy(study, "Area 1");
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-1");
}

BOOST_AUTO_TEST_CASE(check_intra_modal_for_renewable_clusters)
{
	// Creating a study
	Study::Ptr study = new Study();

	study->parameters.intraModal |= timeSeriesRenewable;

	Area* area_1 = addAreaToStudy(study, "Area 1");
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-1");
}
*/