// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyVariableInfo.h"

using Antares::Optimization::ExtractLegacyComponentName;

BOOST_AUTO_TEST_SUITE(test_legacy_variable_info)

BOOST_AUTO_TEST_CASE(single_component_is_extracted)
{
    BOOST_CHECK_EQUAL(ExtractLegacyComponentName("Thermal<nuc-fr>"), "nuc-fr");
    BOOST_CHECK_EQUAL(ExtractLegacyComponentName("Area<west>"), "west");
}

BOOST_AUTO_TEST_CASE(nested_location_keeps_innermost_component)
{
    // Only the last Model<...> component name is kept.
    BOOST_CHECK_EQUAL(
      ExtractLegacyComponentName("Area<area>::ThermalCluster<continuous_generator_candidate>"),
      "continuous_generator_candidate");
}

BOOST_AUTO_TEST_CASE(separators_inside_chevrons_are_preserved)
{
    BOOST_CHECK_EQUAL(ExtractLegacyComponentName("Link<a::b>"), "a::b");
}

BOOST_AUTO_TEST_CASE(no_chevrons_returns_empty)
{
    BOOST_CHECK(ExtractLegacyComponentName("loc").empty());
    BOOST_CHECK(ExtractLegacyComponentName("").empty());
}

BOOST_AUTO_TEST_CASE(empty_chevron_content_returns_empty)
{
    BOOST_CHECK(ExtractLegacyComponentName("Area<>").empty());
}

BOOST_AUTO_TEST_CASE(unmatched_chevrons_return_empty)
{
    BOOST_CHECK(ExtractLegacyComponentName("Area<west").empty());
    BOOST_CHECK(ExtractLegacyComponentName("Area>west").empty());
}

BOOST_AUTO_TEST_SUITE_END()
