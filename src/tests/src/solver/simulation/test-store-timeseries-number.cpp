// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 15/03/23.
//
#define BOOST_TEST_MODULE store - timeseries - number
#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <filesystem>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/writer/result_format.h>
#include <antares/writer/writer_factory.h>
#include "antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h"
#include "antares/solver/simulation/timeseries-numbers.h"

using namespace Antares::Solver;
using namespace Antares::Data;
namespace fs = std::filesystem;

void initializeStudy(Study& study)
{
    study.parameters.derated = false;

    study.runtime.rangeLimits.year[rangeBegin] = 0;
    study.runtime.rangeLimits.year[rangeEnd] = 0;
    study.runtime.rangeLimits.year[rangeCount] = 1;

    study.parameters.renewableGeneration.toAggregated(); // Default

    study.parameters.intraModal = 0;
    study.parameters.interModal = 0;
}

BOOST_AUTO_TEST_CASE(BC_group_TestGroup_has_output_file)
{
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;

    study->bindingConstraintsGroups.add("TestGroup");
    study->bindingConstraintsGroups["TestGroup"]->timeseriesNumbers.reset(1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories,
                                            working_tmp_dir.string().c_str(),
                                            nullptr,
                                            durationCollector);
    fs::path bc_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "TestGroup.txt";

    initializeStudy(*study);
    Antares::Solver::TimeSeriesNumbers::Generate(*study);
    Antares::Solver::TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    bool file_exists = fs::exists(bc_path);
    BOOST_CHECK_EQUAL(file_exists, true);
}

BOOST_AUTO_TEST_CASE(BC_output_ts_numbers_file_for_each_group)
{
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;
    study->bindingConstraintsGroups.add("test1");
    study->bindingConstraintsGroups.add("test2");
    study->bindingConstraintsGroups["test1"]->timeseriesNumbers.reset(1);
    study->bindingConstraintsGroups["test2"]->timeseriesNumbers.reset(1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories,
                                            working_tmp_dir.string().c_str(),
                                            nullptr,
                                            durationCollector);

    initializeStudy(*study);
    Antares::Solver::TimeSeriesNumbers::Generate(*study);

    Antares::Solver::TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    fs::path test1_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test1.txt";
    fs::path test2_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test2.txt";
    BOOST_CHECK_EQUAL(fs::exists(test1_path), true);
    BOOST_CHECK_EQUAL(fs::exists(test2_path), true);
}

BOOST_AUTO_TEST_CASE(BC_timeseries_numbers_store_values)
{
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;
    auto group = study->bindingConstraintsGroups.add("test1");
    auto bc = std::make_shared<BindingConstraint>();
    bc->RHSTimeSeries().resize(10, 10);
    group->add(bc);
    study->bindingConstraintsGroups["test1"]->timeseriesNumbers.reset(1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories,
                                            working_tmp_dir.string().c_str(),
                                            nullptr,
                                            durationCollector);

    initializeStudy(*study);
    Antares::Solver::TimeSeriesNumbers::Generate(*study);

    auto& tsNumbers = study->bindingConstraintsGroups["test1"]->timeseriesNumbers;
    tsNumbers.reset(2);
    tsNumbers[0] = 0;
    tsNumbers[1] = 1;

    Antares::Solver::TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    fs::path test1_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test1.txt";
    BOOST_CHECK_EQUAL(fs::exists(test1_path), true);
    Matrix<uint32_t> out;
    out.loadFromCSVFile(test1_path.string());
    BOOST_CHECK_EQUAL(tsNumbers[0] + 1, out[0][0]);
    BOOST_CHECK_EQUAL(tsNumbers[1] + 1, out[0][1]);
}

BOOST_AUTO_TEST_CASE(sts_area_cluster_inflows_timeseries_numbers_store_values)
{
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;

    // Create an area
    auto area = addAreaToListOfAreas(study->areas, "fr");
    if (area)
    {
        area->createMissingData();
        area->resetToDefaultValues();
    }
    auto& clusters = area->shortTermStorage.storagesByIndex;

    // Add one STS cluster
    clusters.resize(1);
    auto& cluster = clusters[0];
    cluster.id = "sts";

    // Prepare inflow time series
    cluster.series->inflows.resize(10, 10); // [10 timesteps, 10 MC years]

    // Assign time series numbers
    cluster.series->inflowsTSNumbers.reset(2);
    cluster.series->inflowsTSNumbers[0] = 3;
    cluster.series->inflowsTSNumbers[1] = 4;

    // Temp directory and writer
    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories,
                                            working_tmp_dir.string().c_str(),
                                            nullptr,
                                            durationCollector);

    // Init and generate
    initializeStudy(*study);
    // Antares::Solver::TimeSeriesNumbers::Generate(*study);
    Antares::Solver::TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    // Verify the output file for inflows
    fs::path inflow_ts_file = working_tmp_dir / "ts-numbers" / "st-storage" / "fr" / "sts"
                              / "inflows.txt";
    BOOST_CHECK_EQUAL(fs::exists(inflow_ts_file), true);

    Matrix<uint32_t> out;
    out.loadFromCSVFile(inflow_ts_file.string());

    // TimeSeriesNumbers are stored as values + 1
    BOOST_CHECK_EQUAL(out[0][0], 4); // 3 + 1
    BOOST_CHECK_EQUAL(out[0][1], 5); // 4 + 1
}

BOOST_AUTO_TEST_CASE(sts_area_cluster_additional_constraints_timeseries_numbers_store_values)
{
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;

    // Create an area
    auto area = addAreaToListOfAreas(study->areas, "fr");
    if (area)
    {
        area->createMissingData();
        area->resetToDefaultValues();
    }
    auto& clusters = area->shortTermStorage.storagesByIndex;

    // Add one STS cluster
    clusters.resize(1);
    auto& cluster = clusters[0];
    cluster.id = "sts";

    // Add one AdditionalConstraint
    auto constraint = std::make_shared<ShortTermStorage::AdditionalConstraints>();
    constraint->cluster_id = "sts";
    constraint->name = "addc";
    // Prepare time series for the constraint
    constraint->timeSeries.resize(10, 10); // [10 timesteps, 10 MC years]

    // Assign time series numbers
    constraint->timeseriesNumbers.reset(2);
    constraint->timeseriesNumbers[0] = 7;
    constraint->timeseriesNumbers[1] = 8;
    cluster.additionalConstraints.push_back(constraint);

    // Temp directory and writer
    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    Benchmarking::DurationCollector durationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories,
                                            working_tmp_dir.string().c_str(),
                                            nullptr,
                                            durationCollector);

    // Init and store
    initializeStudy(*study);
    Antares::Solver::TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    // Check the output file
    fs::path ac_ts_file = working_tmp_dir / "ts-numbers" / "st-storage" / "fr" / "sts" / "addc.txt";
    BOOST_CHECK_EQUAL(fs::exists(ac_ts_file), true);

    Matrix<uint32_t> out;
    out.loadFromCSVFile(ac_ts_file.string());

    // TimeSeriesNumbers are stored as values + 1
    BOOST_CHECK_EQUAL(out[0][0], 8); // 7 + 1
    BOOST_CHECK_EQUAL(out[0][1], 9); // 8 + 1
}
