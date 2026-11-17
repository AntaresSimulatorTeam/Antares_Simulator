// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/optim-model-filler/VariableBoundOverrides.h"
#include "antares/solver/optimisation/thermalHeuristicsCore.h"

using namespace Antares::LinearProblem;

BOOST_AUTO_TEST_SUITE(thermal_heuristics_core)

BOOST_AUTO_TEST_CASE(fast_guide_is_ceil_of_production_over_unit_size)
{
    const std::vector<double> production = {0., 150., 200., 0.005, 99.};
    std::vector<int> guide(production.size(), -1);
    OPT_CalculerLaCourbeGuideDuNombreDeGroupes(static_cast<int>(production.size()),
                                               [&](int t) { return production[t]; },
                                               100.,
                                               guide);
    const std::vector<int> expected = {0, 2, 2, 0, 1};
    BOOST_CHECK_EQUAL_COLLECTIONS(guide.begin(), guide.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(fast_number_of_units_is_constant_by_blocks)
{
    std::vector<int> guide = {0, 2, 0, 0, 0, 0};
    std::vector<int> numberOfUnits(guide.size());
    BOOST_REQUIRE(OPT_CalculerLeNombreDeGroupesParBlocsDeMUTetMDT(6, 2, guide, numberOfUnits));
    const std::vector<int> expected = {2, 2, 0, 0, 0, 0};
    BOOST_CHECK_EQUAL_COLLECTIONS(numberOfUnits.begin(),
                                  numberOfUnits.end(),
                                  expected.begin(),
                                  expected.end());
}

BOOST_AUTO_TEST_CASE(accurate_enforces_minimum_up_duration)
{
    std::vector<int> minNumberOfUnits = {0, 1, 0, 0, 0, 0};
    const std::vector<int> maxNumberOfUnits(6, 2);
    OPT_ResoudreLePbLineaireDuNombreMinDeGroupesEnMarche(minNumberOfUnits,
                                                         maxNumberOfUnits,
                                                         3,
                                                         1,
                                                         6);
    const std::vector<int> expected = {0, 1, 1, 1, 0, 0};
    BOOST_CHECK_EQUAL_COLLECTIONS(minNumberOfUnits.begin(),
                                  minNumberOfUnits.end(),
                                  expected.begin(),
                                  expected.end());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(variable_bound_overrides)

BOOST_AUTO_TEST_CASE(overrides_tighten_bounds_of_the_right_time_steps)
{
    VariableBoundOverrides overrides;
    overrides.reset(168); // week starting at global time step 168
    overrides.setLowerBounds("thermal", "generation", {10., 20., 30., 40.});
    overrides.setUpperBounds("thermal", "nb_units_on", {1., 1., 1., 1.});
    BOOST_CHECK(overrides.contains("thermal", "generation"));
    BOOST_CHECK(!overrides.contains("thermal", "nb_starting"));

    // Problem covering global time steps 169 and 170 (e.g. a daily optimisation)
    const Api::FillContext ctx{0, 1, 169, 170, 0};

    std::vector<double> lb = {25., 0.};
    std::vector<double> ub = {100., 15.};
    overrides.apply("thermal", "generation", ctx, lb, ub);
    BOOST_CHECK_EQUAL(lb[0], 25.); // the override never loosens a bound
    BOOST_CHECK_EQUAL(lb[1], 30.);
    BOOST_CHECK_EQUAL(ub[0], 100.);
    BOOST_CHECK_EQUAL(ub[1], 30.); // raised to keep the variable feasible

    std::vector<double> lbUnits = {0., 2.};
    std::vector<double> ubUnits = {3., 3.};
    overrides.apply("thermal", "nb_units_on", ctx, lbUnits, ubUnits);
    BOOST_CHECK_EQUAL(ubUnits[0], 1.);
    BOOST_CHECK_EQUAL(ubUnits[1], 1.);
    BOOST_CHECK_EQUAL(lbUnits[1], 1.);

    overrides.reset(0);
    BOOST_CHECK(overrides.empty());
}

BOOST_AUTO_TEST_SUITE_END()
