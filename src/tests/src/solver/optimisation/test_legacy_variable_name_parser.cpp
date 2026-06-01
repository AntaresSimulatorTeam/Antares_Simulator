// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyVariableNameParser.h"

using Antares::Optimization::ParseLegacyVariableName;

BOOST_AUTO_TEST_SUITE(test_legacy_variable_name_parser)

BOOST_AUTO_TEST_CASE(well_formed_name_is_parsed)
{
    const auto parsed = ParseLegacyVariableName("MRG. PRICE::Area<west>::hour<42>");
    BOOST_REQUIRE(parsed);
    BOOST_CHECK_EQUAL(parsed->output, "MRG. PRICE");
    BOOST_CHECK_EQUAL(parsed->component, "west");
    BOOST_CHECK_EQUAL(parsed->timeIndex, 42u);
}

BOOST_AUTO_TEST_CASE(time_index_zero_is_accepted)
{
    const auto parsed = ParseLegacyVariableName("OUT::Area<loc>::hour<0>");
    BOOST_REQUIRE(parsed);
    BOOST_CHECK_EQUAL(parsed->timeIndex, 0u);
}

BOOST_AUTO_TEST_CASE(empty_string_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName(""));
}

BOOST_AUTO_TEST_CASE(no_separator_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("just_a_name"));
}

BOOST_AUTO_TEST_CASE(single_separator_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::hour<1>"));
}

BOOST_AUTO_TEST_CASE(empty_output_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("::loc::hour<1>"));
}

BOOST_AUTO_TEST_CASE(empty_time_part_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::loc::"));
}

BOOST_AUTO_TEST_CASE(missing_angle_brackets_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour42"));
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour<42"));
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour42>"));
}

BOOST_AUTO_TEST_CASE(empty_angle_bracket_value_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour<>"));
}

BOOST_AUTO_TEST_CASE(unit_other_than_hour_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::day<1>"));
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::HOUR<1>"));
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::<1>"));
}

BOOST_AUTO_TEST_CASE(non_numeric_index_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour<abc>"));
    BOOST_CHECK(!ParseLegacyVariableName("output::Area<loc>::hour<->"));
}

BOOST_AUTO_TEST_CASE(component_name_is_extracted_from_chevrons)
{
    // The legacy location is encoded as Model<Component_name>; only the
    // component name inside the chevrons is kept.
    const auto parsed = ParseLegacyVariableName("Generation::Thermal<nuc-fr>::hour<1>");
    BOOST_REQUIRE(parsed);
    BOOST_CHECK_EQUAL(parsed->output, "Generation");
    BOOST_CHECK_EQUAL(parsed->component, "nuc-fr");
    BOOST_CHECK_EQUAL(parsed->timeIndex, 1u);
}

BOOST_AUTO_TEST_CASE(nested_location_keeps_innermost_component)
{
    // Nested location: only the last Model<...> component name is kept.
    const auto parsed = ParseLegacyVariableName(
      "DispatchableProduction::Area<area>::ThermalCluster<continuous_generator_candidate>::hour<1>");
    BOOST_REQUIRE(parsed);
    BOOST_CHECK_EQUAL(parsed->output, "DispatchableProduction");
    BOOST_CHECK_EQUAL(parsed->component, "continuous_generator_candidate");
    BOOST_CHECK_EQUAL(parsed->timeIndex, 1u);
}

BOOST_AUTO_TEST_CASE(extra_separators_split_on_first_and_last)
{
    // Parser uses first and last "::" so multi-segment locations are preserved.
    const auto parsed = ParseLegacyVariableName("output::Link<a::b>::hour<1>");
    BOOST_REQUIRE(parsed);
    BOOST_CHECK_EQUAL(parsed->output, "output");
    BOOST_CHECK_EQUAL(parsed->component, "a::b");
    BOOST_CHECK_EQUAL(parsed->timeIndex, 1u);
}

BOOST_AUTO_TEST_CASE(location_without_chevrons_returns_nullopt)
{
    BOOST_CHECK(!ParseLegacyVariableName("output::loc::hour<1>"));
}

BOOST_AUTO_TEST_SUITE_END()
