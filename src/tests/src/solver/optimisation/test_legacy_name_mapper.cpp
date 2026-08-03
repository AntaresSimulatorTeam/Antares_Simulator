// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/LegacyNameMapper.h"

using Antares::Optimization::LegacyNameMapper;

BOOST_AUTO_TEST_SUITE(test_legacy_name_mapper)

BOOST_AUTO_TEST_CASE(known_unsupplied_energy_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "unsupplied_energy");
}

BOOST_AUTO_TEST_CASE(known_spillage_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("Spillage"), "spilled_energy");
}

BOOST_AUTO_TEST_CASE(empty_name_is_returned_unchanged)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_CASE(mapping_is_case_sensitive)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("unsuppliedenergy"), "unsuppliedenergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("UNSUPPLIEDENERGY"), "UNSUPPLIEDENERGY");
}

BOOST_AUTO_TEST_CASE(known_hydprod_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("HydProd"), "withdrawal_power");
}

BOOST_AUTO_TEST_CASE(known_pumping_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("Pumping"), "injection_power");
}

BOOST_AUTO_TEST_CASE(known_hydro_level_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("HydroLevel"), "level");
}

BOOST_AUTO_TEST_CASE(known_overflow_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("Overflow"), "overflow");
}

BOOST_AUTO_TEST_CASE(known_number_stopping_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("NumberStoppingDispatchableUnits"), "num_units_stopping");
}

BOOST_AUTO_TEST_CASE(known_number_breaking_down_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("NumberBreakingDownDispatchableUnits"), "num_units_failing");
}

BOOST_AUTO_TEST_CASE(known_cost_variation_injection_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("CostVariationInjection"), "injection_variation");
}

BOOST_AUTO_TEST_CASE(known_cost_variation_withdrawal_is_mapped)
{
    const LegacyNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("CostVariationWithdrawal"), "withdrawal_variation");
}

BOOST_AUTO_TEST_SUITE_END()
