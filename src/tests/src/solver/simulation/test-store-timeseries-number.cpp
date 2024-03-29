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
//
// Created by marechaljas on 15/03/23.
//
#define BOOST_TEST_MODULE store-timeseries-number
#define BOOST_TEST_DYN_LINK
#define WIN32_LEAN_AND_MEAN


#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <fstream>
#include "antares/solver/simulation/timeseries-numbers.h"
#include "antares/solver/simulation/BindingConstraintsTimeSeriesNumbersWriter.h"
#include <files-system.h>
#include <antares/writer/writer_factory.h>
#include <antares/writer/result_format.h>
#include <antares/benchmarking/DurationCollector.h>

using namespace Antares::Solver;
using namespace Antares::Data;
namespace fs = std::filesystem;

void initializeStudy(Study &study) {
    study.parameters.derated = false;

    study.runtime = new StudyRuntimeInfos();
    study.runtime->rangeLimits.year[rangeBegin] = 0;
    study.runtime->rangeLimits.year[rangeEnd] = 0;
    study.runtime->rangeLimits.year[rangeCount] = 1;

    study.parameters.renewableGeneration.toAggregated(); // Default

    study.parameters.intraModal = 0;
    study.parameters.interModal = 0;
    study.parameters.timeSeriesToRefresh = 0;
}

BOOST_AUTO_TEST_CASE(BC_group_TestGroup_has_output_file) {
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;

    study->bindingConstraintsGroups.add("TestGroup");
    study->bindingConstraintsGroups["TestGroup"]->timeseriesNumbers.resize(1, 1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::NullDurationCollector nullDurationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories, working_tmp_dir.string().c_str(),
                                              nullptr, nullDurationCollector);
    fs::path bc_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "TestGroup.txt";

    initializeStudy(*study);
    TimeSeriesNumbers::Generate(*study);
    TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    bool file_exists = fs::exists(bc_path);
    BOOST_CHECK_EQUAL(file_exists, true);
}

BOOST_AUTO_TEST_CASE(BC_output_ts_numbers_file_for_each_group) {
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;
    study->bindingConstraintsGroups.add("test1");
    study->bindingConstraintsGroups.add("test2");
    study->bindingConstraintsGroups["test1"]->timeseriesNumbers.resize(1, 1);
    study->bindingConstraintsGroups["test2"]->timeseriesNumbers.resize(1, 1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::NullDurationCollector nullDurationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories, working_tmp_dir.string().c_str(),
                                              nullptr, nullDurationCollector);

    initializeStudy(*study);
    TimeSeriesNumbers::Generate(*study);

    TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    fs::path test1_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test1.txt";
    fs::path test2_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test2.txt";
    BOOST_CHECK_EQUAL(fs::exists(test1_path), true);
    BOOST_CHECK_EQUAL(fs::exists(test2_path), true);
}

BOOST_AUTO_TEST_CASE(BC_timeseries_numbers_store_values) {
    auto study = std::make_shared<Study>();
    study->parameters.storeTimeseriesNumbers = true;
    auto group = study->bindingConstraintsGroups.add("test1");
    auto bc = std::make_shared<BindingConstraint>();
    bc->RHSTimeSeries().resize(10,10);
    group->add(bc);
    study->bindingConstraintsGroups["test1"]->timeseriesNumbers.resize(1, 1);

    auto working_tmp_dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    Benchmarking::NullDurationCollector nullDurationCollector;
    auto resultWriter = resultWriterFactory(ResultFormat::legacyFilesDirectories, working_tmp_dir.string().c_str(),
                                              nullptr, nullDurationCollector);

    initializeStudy(*study);
    TimeSeriesNumbers::Generate(*study);
    Matrix<uint32_t> series(2, 2);
    series[0][0] = 0;
    series[0][1] = 1;
    series[1][0] = 42;
    series[1][1] = 3;
    study->bindingConstraintsGroups["test1"]->timeseriesNumbers = series;

    TimeSeriesNumbers::StoreTimeSeriesNumbersIntoOuput(*study, *resultWriter);

    fs::path test1_path = working_tmp_dir / "ts-numbers" / "bindingconstraints" / "test1.txt";
    BOOST_CHECK_EQUAL(fs::exists(test1_path), true);
    Matrix<uint32_t> out;
    out.loadFromCSVFile(test1_path.string());
    BOOST_CHECK_EQUAL(series[0][0]+1, out[0][0]);
    BOOST_CHECK_EQUAL(series[0][1]+1, out[0][1]);
    BOOST_CHECK_EQUAL(series[1][0]+1, out[1][0]);
    BOOST_CHECK_EQUAL(series[1][1]+1, out[1][1]);
}
