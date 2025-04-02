/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include <antares/optimization-options/options.h>

using namespace Antares::Solver::Optimization;

BOOST_AUTO_TEST_SUITE(OptimizationOptionsTests)

BOOST_AUTO_TEST_CASE(test_insertion_operator)
{
    OptimizationOptions options1{.linearSolver = "scip",
                                 .quadraticSolver = "xpress",
                                 .linearSolverParameters = "opt2 52",
                                 .quadraticSolverParameters = "opt9 23",
                                 .solverLogs = true};
    OptimizationOptions options2{.linearSolver = "sirius",
                                 .quadraticSolver = "coin",
                                 .linearSolverParameters = "opt1 5",
                                 .quadraticSolverParameters = "opt6 7",
                                 .solverLogs = false};
    options1 << options2;
    // All fields of options1 must be overwritten with those of options2, except solverLogs which is
    // a logical OR
    BOOST_CHECK_EQUAL(options1.linearSolver, "sirius");
    BOOST_CHECK_EQUAL(options1.quadraticSolver, "coin");
    BOOST_CHECK_EQUAL(options1.linearSolverParameters, "opt1 5");
    BOOST_CHECK_EQUAL(options1.quadraticSolverParameters, "opt6 7");
    BOOST_CHECK_EQUAL(options1.solverLogs, true);
}

BOOST_AUTO_TEST_SUITE_END()
