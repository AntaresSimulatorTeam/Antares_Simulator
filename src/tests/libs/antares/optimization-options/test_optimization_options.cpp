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
#include "antares/checks/checksOnLPsolver.h"

using namespace Antares;
using namespace Antares::Solver::Optimization;

BOOST_AUTO_TEST_SUITE(OptimizationOptionsTests)

BOOST_AUTO_TEST_CASE(check_default_cmd_line_options)
{
    CmdLineOptimOptions cmdLineOptions;
    BOOST_CHECK_NO_THROW(Check::checkSolverOptions(cmdLineOptions));
}

BOOST_AUTO_TEST_CASE(initializing_options_from_cmd_line_options)
{
    CmdLineOptimOptions cmdLineOptions{.linearSolver = "sirius",
                                       .quadraticSolver = "coin",
                                       .linearSolverParameters = "opt1 5",
                                       .quadraticSolverParameters = "opt6 7",
                                       .solverLogs = false};

    OptimizationOptions options;
    options.firstOpimOptions.solverName = "scip";
    options.quadraticOptimOptions.solverName = "xpress";
    options.firstOpimOptions.solverParameters = "opt2 52";
    options.quadraticOptimOptions.solverParameters = "opt9 23";
    options.solverLogs = true;

    options.initializeWith(cmdLineOptions);
    // All fields of options must be overwritten with those of options2, except solverLogs which is
    // a logical OR
    BOOST_CHECK_EQUAL(options.firstOpimOptions.solverName, "sirius");
    BOOST_CHECK_EQUAL(options.quadraticOptimOptions.solverName, "coin");
    BOOST_CHECK_EQUAL(options.firstOpimOptions.solverParameters, "opt1 5");
    BOOST_CHECK_EQUAL(options.quadraticOptimOptions.solverParameters, "opt6 7");
    BOOST_CHECK_EQUAL(options.solverLogs, true);
}

BOOST_AUTO_TEST_SUITE_END()
