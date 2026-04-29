// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <filesystem>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include "antares/exception/RuntimeError.hpp"

#include "unit_test_utils.h"

using namespace Antares::Solver::LoadFiles;

BOOST_AUTO_TEST_CASE(AcceptMissingFile)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "input" / "data-series");
    BOOST_CHECK_NO_THROW(loadScenarioGroupRepository(studyPath));
}

BOOST_AUTO_TEST_CASE(AcceptEmptyFile)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "input" / "data-series");
    std::ofstream file(studyPath / "input/data-series/modeler-scenariobuilder.dat");
    file.close();
    BOOST_CHECK_NO_THROW(loadScenarioGroupRepository(studyPath));
}

BOOST_AUTO_TEST_CASE(read_a_line)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "input" / "data-series");
    std::ofstream file(studyPath / "input/data-series/modeler-scenariobuilder.dat");
    file << "group1, 0=0\n";
    file.close();
    auto scenarioGroupRepository = loadScenarioGroupRepository(studyPath);
    BOOST_CHECK_EQUAL(scenarioGroupRepository.scenario("group1").getData(0), 0);
}

BOOST_AUTO_TEST_CASE(read_multiple_lines)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "input" / "data-series");
    std::ofstream file(studyPath / "input/data-series/modeler-scenariobuilder.dat");
    file << "group1, 0=0\n";
    file << "group1, 1=1\n";
    file << "group2, 0=2\n";
    file.close();
    auto scenarioGroupRepository = loadScenarioGroupRepository(studyPath);
    BOOST_CHECK_EQUAL(scenarioGroupRepository.scenario("group1").getData(0), 0);
    BOOST_CHECK_EQUAL(scenarioGroupRepository.scenario("group1").getData(1), 1);
    BOOST_CHECK_EQUAL(scenarioGroupRepository.scenario("group2").getData(0), 2);
}

BOOST_AUTO_TEST_CASE(ignore_wrong_lines)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::filesystem::create_directories(studyPath / "input" / "data-series");
    std::ofstream file(studyPath / "input/data-series/modeler-scenariobuilder.dat");
    file << "group1, DF=0\n"; // 'DF' used instead of int
    file << "group1, 1=1\n";
    file.close();
    auto scenarioGroupRepository = loadScenarioGroupRepository(studyPath);
    BOOST_CHECK_EXCEPTION((void)scenarioGroupRepository.scenario("group1").getData(0),
                          Antares::Error::RuntimeError,
                          checkMessage(
                            "In scenario group 'GROUP1', time serie for year 0 does not exist."));
    BOOST_CHECK_EQUAL(scenarioGroupRepository.scenario("group1").getData(1), 1);
}
