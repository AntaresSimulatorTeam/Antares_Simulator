// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test solver simulation things

#define WIN32_LEAN_AND_MEAN

#include <algorithm> // std::adjacent_find

#include <boost/test/unit_test.hpp>

#include <antares/solver/simulation/timeseries-numbers.h>
#include <antares/utils/utils.h>
#include "antares/solver/ts-generator/generator.h"

using namespace Yuni;
using namespace Antares::Data;
using namespace Antares::Solver::TimeSeriesNumbers;

void initializeStudy(Study::Ptr study, unsigned int nbYears = 1)
{
    study->parameters.reset();

    study->runtime.rangeLimits.year[rangeBegin] = 0;
    study->runtime.rangeLimits.year[rangeEnd] = nbYears - 1;

    study->parameters.renewableGeneration.toAggregated(); // Default

    study->parameters.interModal = 0;
}

// ========================
// Add an area to study
// ========================
Area* addAreaToStudy(Study::Ptr study, const std::string& areaName)
{
    Area* area = addAreaToListOfAreas(study->areas, areaName);
    if (area)
    {
        area->createMissingData();
        area->resetToDefaultValues();
    }
    BOOST_CHECK(area);

    return area;
}

// ===========================
// Add a cluster to an area
// ===========================

void addClusterToAreaList(Area* area, std::shared_ptr<ThermalCluster> cluster)
{
    area->thermal.list.addToCompleteList(cluster);
    area->thermal.list.buildIndexes();
}

void addClusterToAreaList(Area* area, std::shared_ptr<RenewableCluster> cluster)
{
    area->renewable.list.addToCompleteList(cluster);
    area->renewable.list.buildIndexes();
}

template<class ClusterType>
std::shared_ptr<ClusterType> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ClusterType>(area);
    cluster->setName(clusterName);

    addClusterToAreaList(area, cluster);

    return cluster;
}

// =======================
// Checks on inter-modal
// =======================

BOOST_AUTO_TEST_CASE(
  one_area__load_wind_thermal_are_turned_to_inter_modal__same_nb_of_ts____check_inter_modal_consistency_OK)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesWind;
    study->parameters.interModal |= timeSeriesThermal;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(5, 1); // Ready made TS for load
    area->wind.series.timeSeries.resize(5, 1); // Ready made TS for wind

    // Thermal
    study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal

    study->parameters.nbTimeSeriesThermal = 5;
    auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
    auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
    BOOST_CHECK(Generate(*study));

    // TS number checks
    uint year = 0;
    uint drawnTsNbForLoad = area->load.series.timeseriesNumbers[year];
    BOOST_CHECK_EQUAL(area->wind.series.timeseriesNumbers[year], drawnTsNbForLoad);
    BOOST_CHECK_EQUAL(thCluster_1->series.timeseriesNumbers[year], drawnTsNbForLoad);
    BOOST_CHECK_EQUAL(thCluster_2->series.timeseriesNumbers[year], drawnTsNbForLoad);
}

BOOST_AUTO_TEST_CASE(
  one_area__load_wind_thermal_are_turned_to_inter_modal__same_nb_of_ts_except_1_for_load_check_inter_modal_consistency_OK)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesWind;
    study->parameters.interModal |= timeSeriesThermal;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(1, 1); // Ready made TS for load
    area->wind.series.timeSeries.resize(5, 1); // Ready made TS for wind

    // ... Thermal
    study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
    study->parameters.nbTimeSeriesThermal = 5;
    // ... ... clusters
    auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
    auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
    BOOST_CHECK(Generate(*study));

    // TS number checks
    uint year = 0;
    uint drawnTsNbForLoad = area->load.series.timeseriesNumbers[year];
    BOOST_CHECK_EQUAL(area->wind.series.timeseriesNumbers[year], drawnTsNbForLoad);
    BOOST_CHECK_EQUAL(thCluster_1->series.timeseriesNumbers[year], drawnTsNbForLoad);
    BOOST_CHECK_EQUAL(thCluster_2->series.timeseriesNumbers[year], drawnTsNbForLoad);
}

BOOST_AUTO_TEST_CASE(
  one_area__load_wind_thermal_are_turned_to_inter_modal__different_nb_of_ts____check_inter_modal_consistency_KO)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesWind;
    study->parameters.interModal |= timeSeriesThermal;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(5, 1); // Ready made TS for load
    area->wind.series.timeSeries.resize(5, 1); // Ready made TS for wind

    // Thermal
    study->parameters.timeSeriesToGenerate |= timeSeriesThermal; // Generated TS for thermal
    study->parameters.nbTimeSeriesThermal = 4;
    // Add 2 clusters to area
    auto thCluster_1 = addClusterToArea<ThermalCluster>(area, "th-cluster-1");
    auto thCluster_2 = addClusterToArea<ThermalCluster>(area, "th-cluster-2");

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
    BOOST_CHECK(not Generate(*study));
}

BOOST_AUTO_TEST_CASE(
  one_area__load_renewable_are_turned_to_inter_modal__same_nb_of_ts____check_inter_modal_consistency_OK)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.renewableGeneration.toClusters();

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesRenewable;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

    auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
    rnCluster_1->series.timeSeries.resize(5, 1);

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    BOOST_CHECK(Generate(*study));

    // TS number checks
    uint year = 0;
    BOOST_CHECK_EQUAL(rnCluster_1->series.timeseriesNumbers[year],
                      area->load.series.timeseriesNumbers[year]);
}

BOOST_AUTO_TEST_CASE(
  one_area__load_renewable_are_turned_to_inter_modal__different_nb_of_ts____check_inter_modal_consistency_KO)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.renewableGeneration.toClusters();

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesRenewable;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

    auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
    rnCluster_1->series.timeSeries.resize(4, 1);

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    BOOST_CHECK(not Generate(*study));
}

BOOST_AUTO_TEST_CASE(
  one_area__load_renewable_are_turned_to_inter_modal_with_respectively_5_1_TS____check_inter_modal_consistency_KO)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    study->parameters.renewableGeneration.toClusters();

    study->parameters.interModal |= timeSeriesLoad;
    study->parameters.interModal |= timeSeriesRenewable;

    Area* area = addAreaToStudy(study, "Area");

    area->load.series.timeSeries.resize(5, 1); // Ready made TS for load

    auto rnCluster_1 = addClusterToArea<RenewableCluster>(area, "rn-cluster-1");
    rnCluster_1->series.timeSeries.resize(1, 1);

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    BOOST_CHECK(Generate(*study));
}

BOOST_AUTO_TEST_CASE(check_all_drawn_ts_numbers_are_bounded_between_0_and_nb_of_ts)
{
    // Creating a study
    auto study = std::make_shared<Study>();
    initializeStudy(study);

    // Generated TS for everyone
    study->parameters.timeSeriesToGenerate |= timeSeriesLoad;
    study->parameters.timeSeriesToGenerate |= timeSeriesWind;
    study->parameters.timeSeriesToGenerate |= timeSeriesSolar;
    study->parameters.timeSeriesToGenerate |= timeSeriesHydro;
    study->parameters.timeSeriesToGenerate |= timeSeriesThermal;

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

    auto thCluster = addClusterToArea<ThermalCluster>(area, "th-cluster");

    area->resizeAllTimeseriesNumbers(1 + study->runtime.rangeLimits.year[rangeEnd]);

    auto bc = study->bindingConstraints.add("dummy");
    bc->group("dummy");
    study->bindingConstraintsGroups.add(bc->group());
    bc->RHSTimeSeries().resize(42, 1);
    study->bindingConstraintsGroups.resizeAllTimeseriesNumbers(
      1 + study->runtime.rangeLimits.year[rangeEnd]);

    TSGenerator::ResizeGeneratedTimeSeries(study->areas, study->parameters);
    BOOST_CHECK(Generate(*study));

    // TS number checks : each energy drawn ts numbers are up-bounded with the number of TS of the
    // related energy
    uint year = 0;
    uint loadTsNumber = area->load.series.timeseriesNumbers[year];
    uint windTsNumber = area->wind.series.timeseriesNumbers[year];
    uint solarTsNumber = area->solar.series.timeseriesNumbers[year];
    uint hydroTsNumber = area->hydro.series->timeseriesNumbers[year];
    uint thermalTsNumber = thCluster->series.timeseriesNumbers[year];
    auto binding_constraints_TS_number = study->bindingConstraintsGroups["dummy"]
                                           ->timeseriesNumbers[year];

    BOOST_CHECK(loadTsNumber < loadNumberOfTs);
    BOOST_CHECK(windTsNumber < windNumberOfTs);
    BOOST_CHECK(solarTsNumber < solarNumberOfTs);
    BOOST_CHECK(hydroTsNumber < hydroNumberOfTs);
    BOOST_CHECK(thermalTsNumber < thermalNumberOfTs);
    BOOST_CHECK_LT(binding_constraints_TS_number, binding_constraints_number_of_TS);
}
