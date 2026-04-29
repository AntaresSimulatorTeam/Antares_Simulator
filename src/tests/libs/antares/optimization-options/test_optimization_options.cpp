// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/optimization-options/options.h>
#include "antares/checks/checksOnLPsolver.h"
#include "antares/exception/LoadingError.hpp"

using namespace Antares;
using namespace Antares::Solver::Optimization;

struct CmdLineOptionsFixture
{
    CmdLineOptimOptions cmdLineOptions;
};

BOOST_FIXTURE_TEST_SUITE(OptimizationOptionsTests, CmdLineOptionsFixture)

BOOST_AUTO_TEST_CASE(check_default_cmd_line_options)
{
    BOOST_CHECK_NO_THROW(Check::checkSolverOptions(cmdLineOptions));
}

BOOST_AUTO_TEST_CASE(when_cmd_line_options_has_linear_solver_name_empty__exception_raised)
{
    cmdLineOptions.linearSolver.clear();
    BOOST_CHECK_THROW(Check::checkSolverOptions(cmdLineOptions), Error::InvalidSolver);
}

BOOST_AUTO_TEST_CASE(when_cmd_line_options_has_quadratic_solver_name_empty__exception_raised)
{
    cmdLineOptions.quadraticSolver.clear();
    BOOST_CHECK_THROW(Check::checkSolverOptions(cmdLineOptions), Error::InvalidSolver);
}

BOOST_AUTO_TEST_CASE(cmd_line_options_has_params_for_both_optims_and_for_optim_1__exception_raised)
{
    cmdLineOptions.linearSolverParameters = "some params for both optims";
    cmdLineOptions.lpSolverParamOptim1 = "some params for optim 1";
    BOOST_CHECK_THROW(Check::checkSolverOptions(cmdLineOptions),
                      Error::IncompatibleLinearSolverParameters);
}

BOOST_AUTO_TEST_CASE(cmd_line_options_asks_for_MILP_with_Sirius_solver__exception_raised)
{
    bool MILPrequired = true;
    BOOST_CHECK_THROW(Check::checkSolverOptions(cmdLineOptions, MILPrequired),
                      Error::IncompatibleMILPOrtoolsSolver);
}

BOOST_AUTO_TEST_CASE(MILP_is_required_and_params_for_optim1_is_supplied__exception_raised)
{
    bool MILPrequired = true;
    cmdLineOptions.linearSolver = "coin"; // When MILP, linear solver cannot be Sirius (default)
    cmdLineOptions.lpSolverParamOptim1 = "Some parameters for solver at optim 1";
    BOOST_CHECK_THROW(Check::checkSolverOptions(cmdLineOptions, MILPrequired),
                      Error::UseMILPsolverWithWrongOptions);
}

BOOST_AUTO_TEST_CASE(initializing_options_from_cmd_line_options)
{
    cmdLineOptions.quadraticSolver = "coin";
    cmdLineOptions.linearSolverParameters = "opt1 5";
    cmdLineOptions.quadraticSolverParameters = "opt6 7";

    OptimizationOptions options;
    options.firstOptimOptions.solverName = "scip";
    options.quadraticOptimOptions.solverName = "xpress";
    options.firstOptimOptions.solverParameters = "opt2 52";
    options.quadraticOptimOptions.solverParameters = "opt9 23";

    options.initializeWith(cmdLineOptions);

    BOOST_CHECK_EQUAL(options.firstOptimOptions.solverName, "sirius");
    BOOST_CHECK_EQUAL(options.quadraticOptimOptions.solverName, "coin");
    BOOST_CHECK_EQUAL(options.firstOptimOptions.solverParameters, "opt1 5");
    BOOST_CHECK_EQUAL(options.quadraticOptimOptions.solverParameters, "opt6 7");
}

BOOST_AUTO_TEST_SUITE_END()
