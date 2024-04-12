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
#define BOOST_TEST_MODULE test-check-input-data tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "antares/checks/checkLoadedInputData.h"
#include "antares/exception/LoadingError.hpp"

BOOST_AUTO_TEST_CASE(test_if_setting_solver_specific_settings_succeeds_do_not_raise_exception)
{
    auto ucMode = Antares::Data::UnitCommitmentMode::ucMILP;

    bool ortoolsUsed = true;
    std::string ortoolsSolver = "scip";
    bool solverLogs = false;
    std::string solverParameters = "lp/threads = 4, lp/presolving = TRUE";

    auto options
      = Antares::Solver::Optimization::OptimizationOptions(ortoolsUsed, ortoolsSolver, solverLogs, solverParameters);
    BOOST_CHECK_NO_THROW(Antares::Check::checkOrtoolsSolverSpecificParameters(ucMode, options));
}

BOOST_AUTO_TEST_CASE(test_if_setting_solver_specific_settings_fails_do_not_raise_exception)
{
    auto ucMode = Antares::Data::UnitCommitmentMode::ucMILP;

    bool ortoolsUsed = true;
    std::string ortoolsSolver = "coin";
    bool solverLogs = false;
    std::string solverParameters = "presolve 1";

    auto options
      = Antares::Solver::Optimization::OptimizationOptions(ortoolsUsed, ortoolsSolver, solverLogs, solverParameters);
    BOOST_CHECK_NO_THROW(Antares::Check::checkOrtoolsSolverSpecificParameters(ucMode, options));
}

BOOST_AUTO_TEST_CASE(test_if_no_ortools_solver_is_used_it_should_exit_normally)
{
    auto ucMode = Antares::Data::UnitCommitmentMode::ucMILP;

    bool ortoolsUsed = false;
    std::string ortoolsSolver = "sirius";
    bool solverLogs = false;
    std::string solverParameters = "INVALID_PARAM 1";

    auto options
      = Antares::Solver::Optimization::OptimizationOptions(ortoolsUsed, ortoolsSolver, solverLogs, solverParameters);
    BOOST_CHECK_NO_THROW(Antares::Check::checkOrtoolsSolverSpecificParameters(ucMode, options));
}