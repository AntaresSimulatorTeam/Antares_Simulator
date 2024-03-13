/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE test solver simulation things
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/timeseries-numbers.h>
#include "antares/solver/ts-generator/generator.h"
#include <antares/utils/utils.h>

#include <algorithm> // std::adjacent_find

using namespace Yuni;
using namespace Antares::Data;
using namespace Antares::Solver::TimeSeriesNumbers;


void initializeStudy(Study::Ptr study, unsigned int nbYears = 1)
{
	study->parameters.derated = false;

	study->runtime = new StudyRuntimeInfos();
	study->runtime->rangeLimits.year[rangeBegin] = 0;
	study->runtime->rangeLimits.year[rangeEnd] = nbYears - 1;
	study->runtime->rangeLimits.year[rangeCount] = nbYears - 1;

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

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
	area->thermal.list.addToCompleteList(cluster);
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
	area->renewable.list.addToCompleteList(cluster);
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
	auto cluster = std::make_shared<ClusterType>(area);
	cluster->setName(clusterName);

	addClusterToAreaList(area, cluster);

	return cluster;
}

BOOST_AUTO_TEST_CASE(test_compare_function_identical_values_OK)
{
    using namespace Antares::Solver::TimeSeriesNumbers;
    std::vector<uint> list = {4, 4, 4, 4};
    BOOST_CHECK(checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_identical_values_and_one_OK)
{
    using namespace Antares::Solver::TimeSeriesNumbers;
    std::vector<uint> list = {4, 4, 4, 4, 1};
    BOOST_CHECK(checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_one_and_identical_values_OK)
{
    using namespace Antares::Solver::TimeSeriesNumbers;
    std::vector<uint> list = {1, 4, 4, 4, 4};
    BOOST_CHECK(checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_two_distinct_values_of_which_one_KO)
{
    using namespace Antares::Solver::TimeSeriesNumbers;
    std::vector<uint> list = {1, 2, 1, 1, 2, 1, 3};
    BOOST_CHECK(!checkAllElementsIdenticalOrOne(list));
}

BOOST_AUTO_TEST_CASE(test_compare_function_three_distinct_values_KO)
{
    using namespace Antares::Solver::TimeSeriesNumbers;
    std::vector<uint> list = {1, 2, 1, 3, 2};
    BOOST_CHECK(!checkAllElementsIdenticalOrOne(list));
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
	area_1->load.series.timeSeries.resize(5, 1);

	// Area 2
	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);
	area_2->load.series.timeSeries.resize(5, 1);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// intra-modal for load : drawn TS numbers in all areas must be equal
	uint year = 0;
	BOOST_CHECK_EQUAL(area_1->load.series.timeseriesNumbers[0][year], area_2->load.series.timeseriesNumbers[0][year]);
}

// =======================
//	Tests on intra-modal
// =======================

static bool intramodal_load_two_areas(unsigned width_area_1, unsigned width_area_2)
{
	// Creating a study
    auto study = std::make_shared<Study>();
	initializeStudy(study);

	study->parameters.intraModal |= timeSeriesLoad;

	// Area 1
	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->load.series.timeSeries.resize(width_area_1, 1);
	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// Area 2
	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->load.series.timeSeries.resize(width_area_2, 1);
	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	return Generate(*study);
}

BOOST_AUTO_TEST_CASE(two_areas_with_respectively_5_and_4_ready_made_ts_on_load___check_intra_modal_consistency_KO)
{
    BOOST_CHECK(!intramodal_load_two_areas(5, 4));
}

BOOST_AUTO_TEST_CASE(two_areas_with_respectively_5_and_1_ready_made_ts_on_load___check_intra_modal_consistency_OK)
{
    BOOST_CHECK(intramodal_load_two_areas(5, 1));
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
	thCluster_11->series.timeSeries.resize(4, 1);
	// ... Area 1 : thermal cluster 2
	auto thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
	thCluster_12->series.timeSeries.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series.timeSeries.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(thCluster_12->series.timeseriesNumbers[0][year], thCluster_11->series.timeseriesNumbers[0][year]);
	BOOST_CHECK_EQUAL(thCluster_21->series.timeseriesNumbers[0][year], thCluster_11->series.timeseriesNumbers[0][year]);
}

BOOST_AUTO_TEST_CASE(two_areas_2_thermal_clusters_with_respectively_4_4_ready_made_ts___check_intra_modal_consistency_OK)
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
	thCluster_11->series.timeSeries.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series.timeSeries.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(thCluster_21->series.timeseriesNumbers[0][year], thCluster_11->series.timeseriesNumbers[0][year]);
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
	thCluster_11->series.timeSeries.resize(4, 1);
	// ... Area 1 : thermal cluster 2
	auto thCluster_12 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-12");
	thCluster_12->series.timeSeries.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : thermal cluster 1
	auto thCluster_21 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-21");
	thCluster_21->series.timeSeries.resize(3, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
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
	rnCluster_11->series.timeSeries.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series.timeSeries.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");
	// ... Area 2 : renewable cluster 1
	auto rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
	rnCluster_21->series.timeSeries.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_12->series.timeseriesNumbers[0][year], rnCluster_11->series.timeseriesNumbers[0][year]);
	BOOST_CHECK_EQUAL(rnCluster_21->series.timeseriesNumbers[0][year], rnCluster_11->series.timeseriesNumbers[0][year]);
}


BOOST_AUTO_TEST_CASE(two_areas_2_renew_clusters_with_respectively_4_4_ready_made_ts___check_intra_modal_consistency_OK)
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
	rnCluster_11->series.timeSeries.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series.timeSeries.resize(4, 1);

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_12->series.timeseriesNumbers[0][year], rnCluster_11->series.timeseriesNumbers[0][year]);
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
	rnCluster_11->series.timeSeries.resize(4, 1);
	// ... Area 1 : renewable cluster 2
	auto rnCluster_12 = addClusterToArea<RenewableCluster>(area_1, "rn-cluster-12");
	rnCluster_12->series.timeSeries.resize(3, 1); // Caution : number of TS different from previous cluster

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// =============
	// Area 2
	// =============
	Area* area_2 = addAreaToStudy(study, "Area 2");

	// ... Area 2 : renewable cluster 1
	auto rnCluster_21 = addClusterToArea<RenewableCluster>(area_2, "rn-cluster-21");
	rnCluster_21->series.timeSeries.resize(4, 1);

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(not Generate(*study));
}

BOOST_AUTO_TEST_CASE(check_intra_modal_on_hydro_max_power_time_series)
{
	unsigned int nbYears = 3;
	auto study = std::make_shared<Study>();
	initializeStudy(study, nbYears);

	study->parameters.intraModal |= timeSeriesHydroMaxPower;

	unsigned int hydroMaxPowerTSsize = 3;

	// Area 1
	Area* area_1 = addAreaToStudy(study, "Area 1");
	area_1->hydro.series->resizeMaxPowerTS(hydroMaxPowerTSsize, 1);
	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// Area 2
	Area* area_2 = addAreaToStudy(study, "Area 2");
	area_2->hydro.series->resizeMaxPowerTS(hydroMaxPowerTSsize, 1);
	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// Area 3
	Area* area_3 = addAreaToStudy(study, "Area 3");
	area_3->hydro.series->resizeMaxPowerTS(hydroMaxPowerTSsize, 1);
	area_3->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	BOOST_CHECK(Generate(*study));

	for (unsigned int year = 0; year < nbYears; year++)
	{
		unsigned int ts_number_1 = area_1->hydro.series->timeseriesNumbersHydroMaxPower[0][year];
		unsigned int ts_number_2 = area_2->hydro.series->timeseriesNumbersHydroMaxPower[0][year];
		unsigned int ts_number_3 = area_3->hydro.series->timeseriesNumbersHydroMaxPower[0][year];

		BOOST_CHECK_EQUAL(ts_number_1, ts_number_2);
		BOOST_CHECK_EQUAL(ts_number_1, ts_number_3);
	}
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
	area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

	// ... Wind
	area->wind.series.timeSeries.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal

	study->parameters.nbTimeSeriesThermal = 5;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	uint drawnTsNbForLoad = area->load.series.timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area->wind.series.timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_1->series.timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_2->series.timeseriesNumbers[0][year], drawnTsNbForLoad);
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
	area->load.series.timeSeries.resize(1, 1); // Ready made TS for load

	// ... Wind
	area->wind.series.timeSeries.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.nbTimeSeriesThermal = 5;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	uint drawnTsNbForLoad = area->load.series.timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area->wind.series.timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_1->series.timeseriesNumbers[0][year], drawnTsNbForLoad);
	BOOST_CHECK_EQUAL(thCluster_2->series.timeseriesNumbers[0][year], drawnTsNbForLoad);
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
	area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

	// ... Wind
	area->wind.series.timeSeries.resize(5, 1);	// Ready made TS for wind

	// ... Thermal
	study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.nbTimeSeriesThermal = 4;
	// ... ... clusters
	auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
	auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
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
	area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series.timeSeries.resize(5, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks
	uint year = 0;
	BOOST_CHECK_EQUAL(rnCluster_1->series.timeseriesNumbers[0][year], area->load.series.timeseriesNumbers[0][year]);
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
	area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series.timeSeries.resize(4, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
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
	area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

	// ... Renewable
	// ... ... clusters
	auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
	rnCluster_1->series.timeSeries.resize(1, 1);

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
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
	study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
	study->parameters.timeSeriesToRefresh |= timeSeriesThermal;
	study->parameters.nbTimeSeriesThermal = 5;

	// ===============
	// Area 1
	// ===============
	Area* area_1 = addAreaToStudy(study, "Area 1");
	// ... Load
	area_1->load.series.timeSeries.resize(5, 1); // Ready made TS for load
	// ... Wind
	area_1->wind.series.timeSeries.resize(5, 1);	// Ready made TS for wind
	// ... Thermal
	auto thCluster_area_1 = addClusterToArea<ThermalCluster>(area_1, "th-cluster-area-1");

	area_1->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	// ===============
	// Area 2
	// ===============
	Area* area_2 = addAreaToStudy(study, "Area 2");
	// ... Load
	area_2->load.series.timeSeries.resize(5, 1); // Ready made TS for load
	// ... Wind
	area_2->wind.series.timeSeries.resize(5, 1);	// Ready made TS for wind
	// ... Thermal
	auto thCluster_area_2 = addClusterToArea<ThermalCluster>(area_2, "th-cluster-area-2");

	area_2->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks : all intra-modal & inter-modal modes have get the same ts number :
	// - inside an area
	// - for all areas
	uint year = 0;
	uint referenceLoadTsNumber = area_1->load.series.timeseriesNumbers[0][year];
	BOOST_CHECK_EQUAL(area_2->load.series.timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(area_1->wind.series.timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(area_2->wind.series.timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(thCluster_area_1->series.timeseriesNumbers[0][year], referenceLoadTsNumber);
	BOOST_CHECK_EQUAL(thCluster_area_2->series.timeseriesNumbers[0][year], referenceLoadTsNumber);
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
	uint binding_constraints_number_of_TS = 42;

	study->parameters.nbTimeSeriesLoad = loadNumberOfTs;
	study->parameters.nbTimeSeriesWind = windNumberOfTs;
	study->parameters.nbTimeSeriesSolar = solarNumberOfTs;
	study->parameters.nbTimeSeriesHydro = hydroNumberOfTs;
	study->parameters.nbTimeSeriesThermal = thermalNumberOfTs;

	Area* area = addAreaToStudy(study, "Area");

	// ... Thermal
	auto thCluster = addClusterToArea<ThermalCluster>(area, "th-cluster");

	area->resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);
    auto bc = study->bindingConstraints.add("dummy");
    bc->group("dummy");
    study->bindingConstraintsGroups.add(bc->group());
    bc->RHSTimeSeries().resize(42, 1);
    study->bindingConstraintsGroups.resizeAllTimeseriesNumbers(1 + study->runtime->rangeLimits.year[rangeEnd]);

	TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
	BOOST_CHECK(Generate(*study));

	// TS number checks : each energy drawn ts numbers are up-bounded with the number of TS of the related energy
	uint year = 0;
	uint loadTsNumber = area->load.series.timeseriesNumbers[0][year];
	uint windTsNumber = area->wind.series.timeseriesNumbers[0][year];
	uint solarTsNumber = area->solar.series.timeseriesNumbers[0][year];
	uint hydroTsNumber = area->hydro.series->timeseriesNumbers[0][year];
	uint thermalTsNumber = thCluster->series.timeseriesNumbers[0][year];
	auto binding_constraints_TS_number = study->bindingConstraintsGroups["dummy"]->timeseriesNumbers[0][year];

	BOOST_CHECK(loadTsNumber < loadNumberOfTs);
	BOOST_CHECK(windTsNumber < windNumberOfTs);
	BOOST_CHECK(solarTsNumber < solarNumberOfTs);
	BOOST_CHECK(hydroTsNumber < hydroNumberOfTs);
	BOOST_CHECK(thermalTsNumber < thermalNumberOfTs);
    BOOST_CHECK_LT(binding_constraints_TS_number, binding_constraints_number_of_TS);
}

BOOST_AUTO_TEST_CASE(split_string_ts_cluster_gen)
{
    char delimiter1 = ';';
    char delimiter2 = '.';

    using stringPair = std::pair<std::string, std::string>;
    std::vector<stringPair> v;

    // only one pair of area cluster
    v = splitStringIntoPairs("abc.def", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));

    // two pairs
    v = splitStringIntoPairs("abc.def;ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));
    BOOST_CHECK(v[1] == stringPair("ghi", "jkl"));

    // first pair isn't valid
    v = splitStringIntoPairs("abcdef;ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("ghi", "jkl"));

    // second pair isn't valid
    v = splitStringIntoPairs("abc.def;ghijkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def"));

    // no semi colon
    v = splitStringIntoPairs("abc.def.ghi.jkl", delimiter1, delimiter2);
    BOOST_CHECK(v[0] == stringPair("abc", "def.ghi.jkl"));

    // no separator
    v.clear();
    v = splitStringIntoPairs("abcdef", delimiter1, delimiter2);
    BOOST_CHECK(v.empty());
}

