#define BOOST_TEST_MODULE test solver simulation things

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <study.h>
#include <timeseries-numbers.h>

#include <algorithm> // std::adjacent_find

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

	study->parameters.renewableGeneration.toAggregated(); // Default

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
void addClusterToAreaList(Area* area, std::shared_ptr<ClusterType> cluster)
{}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
	area->thermal.clusters.push_back(cluster.get());
	area->thermal.list.add(cluster);
	area->thermal.list.mapping[cluster->id()] = cluster;
}

template<>
void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
	area->renewable.clusters.push_back(cluster.get());
	area->renewable.list.add(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
	auto cluster = std::make_shared<ClusterType>(area);
	cluster->setName(clusterName);
	cluster->series = new DataSeriesCommon();

	addClusterToAreaList(area, cluster);

	return cluster;
}

BOOST_AUTO_TEST_CASE(test_compare_function_identical_values_OK)
{
    std::vector<uint> list = {4, 4, 4, 4};
    auto find_result = std::adjacent_find(
      list.begin(), list.end(), Antares::Solver::TimeSeriesNumbers::compareWidth);
    BOOST_CHECK(find_result == list.end());
}

BOOST_AUTO_TEST_CASE(test_compare_function_two_distinct_values_of_which_one_OK)
{
    std::vector<uint> list = {1, 2, 1, 1, 2};
    auto find_result = std::adjacent_find(
      list.begin(), list.end(), Antares::Solver::TimeSeriesNumbers::compareWidth);
    BOOST_CHECK(find_result == list.end());
}

BOOST_AUTO_TEST_CASE(test_compare_function_three_distinct_values_KO)
{
    std::vector<uint> list = {1, 2, 1, 3, 2};
    auto find_result = std::adjacent_find(
      list.begin(), list.end(), Antares::Solver::TimeSeriesNumbers::compareWidth);
    BOOST_CHECK(find_result != list.end());
}

BOOST_AUTO_TEST_CASE(two_areas_with_5_ready_made_ts_on_load___check_intra_modal_consistency_OK)
{
	// Creating a study
    auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesLoad;

	// Area 1
	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);
	area_1->load.series->series.resize(5, 1);

	// Area 2
	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);
	area_2->load.series->series.resize(5, 1);

	BOOST_CHECK(Generate(*study));

	// intra-modal for load : drawn TS numbers in all areas must be equal
	uint year = 0;
	BOOST_CHECK_EQUAL(area_1->load.series->timeseriesNumbers[0][year], area_2->load.series->timeseriesNumbers[0][year]);
}

static bool intermodal_load_two_areas(unsigned width_area_1, unsigned width_area_2)
{
	// Creating a study
    auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesLoad;

	// Area 1
	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->load.series->series.resize(width_area_1, 1);
	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// Area 2
	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->load.series->series.resize(width_area_2, 1);
	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	return Generate(*study);
}

BOOST_AUTO_TEST_CASE(two_areas_with_respectively_5_and_4_ready_made_ts_on_load___check_intra_modal_consistency_KO)
{
    BOOST_CHECK(!intermodal_load_two_areas(5, 4));
}

BOOST_AUTO_TEST_CASE(two_areas_with_respectively_5_and_1_ready_made_ts_on_load___check_intra_modal_consistency_OK)
{
    BOOST_CHECK(intermodal_load_two_areas(5, 1));
}

BOOST_AUTO_TEST_CASE(two_areas_3_thermal_clusters_with_same_number_of_ready_made_ts___check_intra_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesThermal;

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : thermal cluster 1
	auto thCluster_11 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-11");
	thCluster_11->series->series.resize(4, 1);
	// ... Area 1 : thermal cluster 2
	auto thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
	thCluster_12->series->series.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series->series.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(thCluster_12->series->timeseriesNumbers[0][year], thCluster_11->series->timeseriesNumbers[0][year]);
	BOOST_CHECK_EQUAL(thCluster_21->series->timeseriesNumbers[0][year], thCluster_11->series->timeseriesNumbers[0][year]);
}

BOOST_AUTO_TEST_CASE(two_areas_3_thermal_clusters_with_respectively_4_1_4_ready_made_ts___check_intra_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesThermal;

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : thermal cluster 1
	auto thCluster_11 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-11");
	thCluster_11->series->series.resize(4, 1);
	// ... Area 1 : thermal cluster 2
	auto thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
	thCluster_12->series->series.resize(1, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series->series.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(thCluster_21->series->timeseriesNumbers[0][year], thCluster_11->series->timeseriesNumbers[0][year]);
}

BOOST_AUTO_TEST_CASE(two_areas_3_thermal_clusters_with_different_number_of_ready_made_ts___check_intra_modal_consistency_KO)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesThermal;

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : thermal cluster 1
	auto thCluster_11 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-11");
	thCluster_11->series->series.resize(4, 1);
	// ... Area 1 : thermal cluster 2
	auto thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
	thCluster_12->series->series.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series->series.resize(3, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);


	BOOST_CHECK(not Generate(*study));
}

BOOST_AUTO_TEST_CASE(two_areas_3_renew_clusters_with_same_number_of_ready_made_ts___check_intra_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesRenewable;
	study->parameters.renewableGeneration.toClusters();

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : renewable cluster 1
	auto rnCluster_11 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-11");
	rnCluster_11->series->series.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series->series.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");
	// ... Area 2 : renewable cluster 1
	auto rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
	rnCluster_21->series->series.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_12->series->timeseriesNumbers[0][year], rnCluster_11->series->timeseriesNumbers[0][year]);
	BOOST_CHECK_EQUAL(rnCluster_21->series->timeseriesNumbers[0][year], rnCluster_11->series->timeseriesNumbers[0][year]);
}


BOOST_AUTO_TEST_CASE(two_areas_3_renew_clusters_with_respectively_4_4_1_ready_made_ts___check_intra_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesRenewable;
	study->parameters.renewableGeneration.toClusters();

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : renewable cluster 1
	auto rnCluster_11 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-11");
	rnCluster_11->series->series.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series->series.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");
	// ... Area 2 : renewable cluster 1
	auto rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
	rnCluster_21->series->series.resize(1, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_12->series->timeseriesNumbers[0][year], rnCluster_11->series->timeseriesNumbers[0][year]);
}

BOOST_AUTO_TEST_CASE(two_areas_3_renew_clusters_with_different_number_of_ready_made_ts___check_intra_modal_consistency_KO)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesRenewable;
	study->parameters.renewableGeneration.toClusters();

	// =============
	// Area 1
	// =============
	Area* area_1 = addAreaToStudy(study, "Area 1");

	// ... Area 1 : renewable cluster 1
	auto rnCluster_11 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-11");
	rnCluster_11->series->series.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series->series.resize(3, 1); // Caution : number of TS different from previous cluster 

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : renewable cluster 1
	auto rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
	rnCluster_21->series->series.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(not Generate(*study));
}

// =======================
// Checks on inter-modal
// =======================

BOOST_AUTO_TEST_CASE(one_area__load_wind_thermal_are_turned_to_inter_modal__same_nb_of_ts____check_inter_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesWind;
	study->parameters.interModal |= timeSeriesThermal;

	// Area
	Area* area = addAreaToStudy(study, "Area");
	
	// ... Load
	area->load.series->series.resize(5, 1); // Ready made TS for load

	// ... Wind
	area->wind.series->series.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.nbTimeSeriesThermal = 5;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	uint drawnTsNbForLoad = area->load.series->timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area->wind.series->timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_1->series->timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_2->series->timeseriesNumbers[0][year], drawnTsNbForLoad);
}

BOOST_AUTO_TEST_CASE(one_area__load_wind_thermal_are_turned_to_inter_modal__same_nb_of_ts_except_1_for_load_check_inter_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesWind;
	study->parameters.interModal |= timeSeriesThermal;

	// Area
	Area* area = addAreaToStudy(study, "Area");

	// ... Load
	area->load.series->series.resize(1, 1); // Ready made TS for load

	// ... Wind
	area->wind.series->series.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.nbTimeSeriesThermal = 5;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	uint drawnTsNbForLoad = area->load.series->timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area->wind.series->timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_1->series->timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_2->series->timeseriesNumbers[0][year], drawnTsNbForLoad);
}

BOOST_AUTO_TEST_CASE(one_area__load_wind_thermal_are_turned_to_inter_modal__different_nb_of_ts____check_inter_modal_consistency_KO)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesWind;
	study->parameters.interModal |= timeSeriesThermal;

	// Area
	Area* area = addAreaToStudy(study, "Area");

	// ... Load
	area->load.series->series.resize(5, 1); // Ready made TS for load

	// ... Wind
	area->wind.series->series.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.nbTimeSeriesThermal = 4;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(not Generate(*study));
}


BOOST_AUTO_TEST_CASE(one_area__load_renewable_are_turned_to_inter_modal__same_nb_of_ts____check_inter_modal_consistency_OK)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.renewableGeneration.toClusters();

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesRenewable;

	// Area
	Area* area = addAreaToStudy(study, "Area");

	// ... Load
	area->load.series->series.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series->series.resize(5, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_1->series->timeseriesNumbers[0][year], area->load.series->timeseriesNumbers[0][year]);
}

BOOST_AUTO_TEST_CASE(one_area__load_renewable_are_turned_to_inter_modal__different_nb_of_ts____check_inter_modal_consistency_KO)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.renewableGeneration.toClusters();

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesRenewable;

	// Area
	Area* area = addAreaToStudy(study, "Area");

	// ... Load
	area->load.series->series.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series->series.resize(4, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(not Generate(*study));
}

BOOST_AUTO_TEST_CASE(one_area__load_renewable_are_turned_to_inter_modal_with_respectively_5_1_TS____check_inter_modal_consistency_KO)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.renewableGeneration.toClusters();

	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesRenewable;

	// Area
	Area* area = addAreaToStudy(study, "Area");

	// ... Load
	area->load.series->series.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series->series.resize(1, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));
}

// ========================================================
// Checks when both intra-modal and inter-modal are on
// ========================================================

BOOST_AUTO_TEST_CASE(load_wind_thermal_in_intra_and_inter_modal____check_all_ts_numbers_are_equal)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	// Intra-modal for load, wind and thermal
	study->parameters.intraModal |= timeSeriesLoad;
	study->parameters.intraModal |= timeSeriesWind;
	study->parameters.intraModal |= timeSeriesThermal;

	// Inter-modal for load, wind and thermal
	study->parameters.interModal |= timeSeriesLoad;
	study->parameters.interModal |= timeSeriesWind;
	study->parameters.interModal |= timeSeriesThermal;

	// Generated TS for thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal;
	study->parameters.nbTimeSeriesThermal = 5;

	// ===============
	// Area 1
	// ===============
	Area* area_1 = addAreaToStudy(study, "Area 1");
	// ... Load
	area_1->load.series->series.resize(5, 1); // Ready made TS for load
	// ... Wind
	area_1->wind.series->series.resize(5, 1);	// Ready made TS for wind
	// ... Thermal
	auto thCluster_area_1 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-area-1");

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// ===============
	// Area 2
	// ===============
	Area* area_2 = addAreaToStudy(study, "Area 2");
	// ... Load
	area_2->load.series->series.resize(5, 1); // Ready made TS for load
	// ... Wind
	area_2->wind.series->series.resize(5, 1);	// Ready made TS for wind
	// ... Thermal
	auto thCluster_area_2 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-area-2");

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks : all intra-modal & inter-modal modes have get the same ts number :
	// - inside an area
	// - for all areas
	uint year = 0;
	uint referenceLoadTsNumber = area_1->load.series->timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area_2->load.series->timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(area_1->wind.series->timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(area_2->wind.series->timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(thCluster_area_1->series->timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(thCluster_area_2->series->timeseriesNumbers[0][year], referenceLoadTsNumber);
}


BOOST_AUTO_TEST_CASE(check_all_drawn_ts_numbers_are_bounded_between_0_and_nb_of_ts)
{
	// Creating a study
	auto study = std::make_shared<Study>();
	initializeStudy(study);

	// Generated TS for everyone
	study->parameters.timeSeriesToRefresh |= timeSeriesLoad;
	study->parameters.timeSeriesToRefresh |= timeSeriesWind;
	study->parameters.timeSeriesToRefresh |= timeSeriesSolar;
	study->parameters.timeSeriesToRefresh |= timeSeriesHydro;
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal;

	// Number of TS for each energy
	uint loadNumberOfTs = 10;
	uint windNumberOfTs = 3;
	uint solarNumberOfTs = 7;
	uint hydroNumberOfTs = 9;
	uint thermalNumberOfTs = 5;

	study->parameters.nbTimeSeriesLoad = loadNumberOfTs;
	study->parameters.nbTimeSeriesWind = windNumberOfTs;
	study->parameters.nbTimeSeriesSolar = solarNumberOfTs;
	study->parameters.nbTimeSeriesHydro = hydroNumberOfTs;
	study->parameters.nbTimeSeriesThermal = thermalNumberOfTs;

	Area* area = addAreaToStudy(study, "Area");

	// ... Thermal
	auto thCluster = addClusterToArea<ThermalCluster>(area, "th-cluster");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	// TS number checks : each energy drawn ts numbers are up-bounded with the number of TS of the related energy
	uint year = 0;
	uint loadTsNumber = area->load.series->timeseriesNumbers[0][year];
	uint windTsNumber = area->wind.series->timeseriesNumbers[0][year];
	uint solarTsNumber = area->solar.series->timeseriesNumbers[0][year];
	uint hydroTsNumber = area->hydro.series->timeseriesNumbers[0][year];
	uint thermalTsNumber = thCluster->series->timeseriesNumbers[0][year];

	BOOST_CHECK(loadTsNumber < loadNumberOfTs);
	BOOST_CHECK(windTsNumber < windNumberOfTs);
	BOOST_CHECK(solarTsNumber < solarNumberOfTs);
	BOOST_CHECK(hydroTsNumber < hydroNumberOfTs);
	BOOST_CHECK(thermalTsNumber < thermalNumberOfTs);
}
