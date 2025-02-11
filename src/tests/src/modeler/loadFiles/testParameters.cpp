/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
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
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
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
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
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
