// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/solver/optimisation/VariableNameMapper.h"

using Antares::Optimization::VariableNameMapper;

namespace
{
VariableNameMapper makeMapper()
{
    return VariableNameMapper({
      {"UnsuppliedEnergy", "unsupplied_energy"},
      {"Spillage", "spillaged_energy"},
    });
}
} // namespace

BOOST_AUTO_TEST_SUITE(test_variable_name_mapper)

BOOST_AUTO_TEST_CASE(known_unsupplied_energy_is_mapped)
{
    const VariableNameMapper mapper = makeMapper();
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "unsupplied_energy");
}

BOOST_AUTO_TEST_CASE(known_spillage_is_mapped)
{
    const VariableNameMapper mapper = makeMapper();
    BOOST_CHECK_EQUAL(mapper.mapOutput("Spillage"), "spillaged_energy");
}

BOOST_AUTO_TEST_CASE(unknown_name_is_returned_unchanged)
{
    const VariableNameMapper mapper = makeMapper();
    BOOST_CHECK_EQUAL(mapper.mapOutput("HydProd"), "HydProd");
}

BOOST_AUTO_TEST_CASE(empty_name_is_returned_unchanged)
{
    const VariableNameMapper mapper = makeMapper();
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_CASE(mapping_is_case_sensitive)
{
    const VariableNameMapper mapper = makeMapper();
    BOOST_CHECK_EQUAL(mapper.mapOutput("unsuppliedenergy"), "unsuppliedenergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("UNSUPPLIEDENERGY"), "UNSUPPLIEDENERGY");
}

BOOST_AUTO_TEST_CASE(default_mapper_returns_every_name_unchanged)
{
    const VariableNameMapper mapper;
    BOOST_CHECK_EQUAL(mapper.mapOutput("UnsuppliedEnergy"), "UnsuppliedEnergy");
    BOOST_CHECK_EQUAL(mapper.mapOutput("Spillage"), "Spillage");
    BOOST_CHECK_EQUAL(mapper.mapOutput(""), "");
}

BOOST_AUTO_TEST_SUITE_END()
