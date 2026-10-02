// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE hebdo_area_price_provider

#include <boost/test/unit_test.hpp>

#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include "antares/solver/optimisation/HebdoAreaPriceProvider.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Optimization;
using namespace Antares::LinearProblem::MpsolverImpl;

namespace
{
constexpr unsigned hour = 0;

// Problem: min 2 * x s.t. x >= 3 (balance of "area1"), 0 <= x <= 10.
// For the LP, the marginal cost of the balance is 2: the price is the opposite of the
// solver's dual (same sign convention as the legacy extra-outputs), i.e. positive.
// For the MILP, x is an integer: duals are not available and the price must be 0.
void fillProblem(Antares::LinearProblem::Api::ILinearProblem& problem, bool integer)
{
    auto* x = problem.addVariable(0., 10., integer, "x");
    problem.setObjectiveCoefficient(x, 2.);
    problem.setMinimization();
    auto* balance = problem.addConstraint(3., problem.infinity(), "balance");
    balance->setCoefficient(x, 1.);
}

void fillHebdo(PROBLEME_HEBDO& hebdo)
{
    static const char* area = "area1";
    hebdo.NomsDesPays = {area};
    hebdo.CorrespondanceCntNativesCntOptim.resize(1);
    hebdo.CorrespondanceCntNativesCntOptim[hour].NumeroDeContrainteDesBilansPays = {0};
}
} // namespace

BOOST_AUTO_TEST_SUITE(hebdo_area_price_provider)

BOOST_AUTO_TEST_CASE(lp_returns_opposite_of_balance_dual)
{
    OrtoolsLinearProblem problem(false, "sirius");
    fillProblem(problem, false);
    problem.solve(false);

    PROBLEME_HEBDO hebdo;
    fillHebdo(hebdo);

    HebdoAreaPriceProvider provider(hebdo, problem);
    BOOST_CHECK_CLOSE(provider.getAreaPrice("area1", hour), 2., 1e-6);
}

BOOST_AUTO_TEST_CASE(milp_returns_zero_without_reading_duals)
{
    OrtoolsLinearProblem problem(true, "scip");
    fillProblem(problem, true);
    problem.solve(false);
    BOOST_REQUIRE(!problem.isLP());

    PROBLEME_HEBDO hebdo;
    fillHebdo(hebdo);

    HebdoAreaPriceProvider provider(hebdo, problem);
    BOOST_CHECK_EQUAL(provider.getAreaPrice("area1", hour), 0.);
}

BOOST_AUTO_TEST_CASE(milp_solver_with_only_continuous_variables_returns_zero)
{
    // A MIP solver is used as soon as the study asks for integer variables, even if none
    // ends up in this particular problem: dual_value() is still unavailable.
    OrtoolsLinearProblem problem(true, "scip");
    fillProblem(problem, false);
    problem.solve(false);

    PROBLEME_HEBDO hebdo;
    fillHebdo(hebdo);

    HebdoAreaPriceProvider provider(hebdo, problem);
    BOOST_CHECK_EQUAL(provider.getAreaPrice("area1", hour), 0.);
}

BOOST_AUTO_TEST_CASE(unknown_area_returns_zero)
{
    OrtoolsLinearProblem problem(false, "sirius");
    fillProblem(problem, false);
    problem.solve(false);

    PROBLEME_HEBDO hebdo;
    fillHebdo(hebdo);

    HebdoAreaPriceProvider provider(hebdo, problem);
    BOOST_CHECK_EQUAL(provider.getAreaPrice("unknown", hour), 0.);
}

BOOST_AUTO_TEST_SUITE_END()
