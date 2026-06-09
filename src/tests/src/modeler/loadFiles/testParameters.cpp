// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>

#include "files-system.h"

BOOST_AUTO_TEST_CASE(read_parameters)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver: sirius
        solver-logs: false
        solver-parameters: PRESOLVE 1
        no-output: true
        export-mps: true
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
    BOOST_CHECK_EQUAL(params.exportMps, true);
}

BOOST_AUTO_TEST_CASE(read_parameters_out_of_order)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver-logs: false
        solver: sirius
        solver-parameters: PRESOLVE 1
        no-output: true
        export-mps: false
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
    BOOST_CHECK_EQUAL(params.exportMps, false);
}

BOOST_AUTO_TEST_CASE(parameters_missing)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver-logs: false
        no-output: true
    )";
    paramStream.close();

    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadParameters(studyPath), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(file_missing)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadParameters(studyPath), std::runtime_error);
}
