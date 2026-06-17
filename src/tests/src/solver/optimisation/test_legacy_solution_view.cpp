// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacySolutionView.h"

using Antares::Optimization::LegacySolutionView;
using Antares::Optimization::LegacyVariableInfo;

namespace
{
struct Fixture
{
    // Three recorded variables and one unnamed slot (index 2), mimicking a
    // problem where not every variable gets legacy info.
    Fixture()
    {
        info.resize(4);
        info[0] = LegacyVariableInfo{"UnsuppliedEnergy", "area1", 7};
        info[1] = LegacyVariableInfo{"Spillage", "area1", 7};
        info[3] = LegacyVariableInfo{"UnsuppliedEnergy", "area2", 7};
    }

    std::vector<std::optional<LegacyVariableInfo>> info;
    std::vector<double> values = {52., 0., -1., 13.};
    std::vector<double> costs = {10000., 4., -1., 20000.};
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(test_legacy_solution_view, Fixture)

BOOST_AUTO_TEST_CASE(value_of_recorded_variable_is_found)
{
    const LegacySolutionView view(info, values, costs);
    BOOST_REQUIRE(view.value("UnsuppliedEnergy", "area1", 7).has_value());
    BOOST_CHECK_EQUAL(*view.value("UnsuppliedEnergy", "area1", 7), 52.);
    BOOST_CHECK_EQUAL(*view.value("Spillage", "area1", 7), 0.);
}

BOOST_AUTO_TEST_CASE(linear_cost_of_recorded_variable_is_found)
{
    const LegacySolutionView view(info, values, costs);
    BOOST_REQUIRE(view.linearCost("UnsuppliedEnergy", "area1", 7).has_value());
    BOOST_CHECK_EQUAL(*view.linearCost("UnsuppliedEnergy", "area1", 7), 10000.);
}

BOOST_AUTO_TEST_CASE(same_name_is_distinguished_by_component)
{
    const LegacySolutionView view(info, values, costs);
    BOOST_CHECK_EQUAL(*view.value("UnsuppliedEnergy", "area2", 7), 13.);
    BOOST_CHECK_EQUAL(*view.linearCost("UnsuppliedEnergy", "area2", 7), 20000.);
}

BOOST_AUTO_TEST_CASE(unknown_name_component_or_time_is_not_found)
{
    const LegacySolutionView view(info, values, costs);
    BOOST_CHECK(!view.value("DispatchableProduction", "area1", 7).has_value());
    BOOST_CHECK(!view.value("UnsuppliedEnergy", "area3", 7).has_value());
    BOOST_CHECK(!view.value("UnsuppliedEnergy", "area1", 8).has_value());
    BOOST_CHECK(!view.linearCost("UnsuppliedEnergy", "area1", 8).has_value());
}

BOOST_AUTO_TEST_CASE(variables_without_recorded_info_are_not_indexed)
{
    const LegacySolutionView view(info, values, costs);
    BOOST_CHECK(!view.value("", "", 0).has_value());
}

BOOST_AUTO_TEST_SUITE_END()
