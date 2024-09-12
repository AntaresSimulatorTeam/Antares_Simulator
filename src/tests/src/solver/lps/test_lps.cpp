/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE test_translator
#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/solver/lps/LpsFromAntares.h>

using namespace Antares::Solver;

BOOST_AUTO_TEST_CASE(new_lps_is_empty)
{
    LpsFromAntares lps;
    BOOST_CHECK(lps.empty());
}

BOOST_AUTO_TEST_CASE(lps_with_only_constant_data_is_empty)
{
    LpsFromAntares lps;
    lps.setConstantData(ConstantDataFromAntares());
    BOOST_CHECK(lps.empty());
}

BOOST_AUTO_TEST_CASE(lps_with_no_variabled_is_empty)
{
    LpsFromAntares lps;
    lps.constantProblemData.VariablesCount = 0;
    lps.addWeeklyData({0, 0}, WeeklyDataFromAntares());
    BOOST_CHECK(lps.empty());
}

BOOST_AUTO_TEST_CASE(lps_with_both_constant_and_weekly_data_is_not_empty)
{
    LpsFromAntares lps;
    lps.constantProblemData.VariablesCount = 65;
    lps.addWeeklyData({0, 0}, WeeklyDataFromAntares());
    BOOST_CHECK(!lps.empty());
}

BOOST_AUTO_TEST_CASE(replace_const_data)
{
    LpsFromAntares lps;
    ConstantDataFromAntares var;
    var.VariablesCount = 42;
    lps.setConstantData(var);
    BOOST_CHECK(lps.constantProblemData.VariablesCount == 42);
}

// Add weekly data for week 1 year 1
BOOST_AUTO_TEST_CASE(add_weekly_data_for_week_1_year_1)
{
    LpsFromAntares lps;
    WeeklyDataFromAntares w;
    w.RHS.push_back(43);
    lps.addWeeklyData({1, 1}, w);
    BOOST_CHECK(lps.weeklyData({1, 1}).RHS.size() != WeeklyDataFromAntares().RHS.size());
}
