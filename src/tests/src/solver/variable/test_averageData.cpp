/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#define WIN32_LEAN_AND_MEAN

#include <algorithm>

#include <boost/test/unit_test.hpp>

#include "antares/solver/variable/storage/averagedata.h"

constexpr double TOLERANCE = 1.e-6;

using namespace Antares::Solver::Variable;

BOOST_AUTO_TEST_SUITE(AverageData_suite)

auto all_close = [](auto val, double expected, double tol = 1e-6)
{ return std::abs(val - expected) < tol; };

BOOST_AUTO_TEST_CASE(Merge_WithEmptyIntermediateValues_DoesNotChangeAverages)
{
    R::AllYears::AverageData avgData(1);
    IntermediateValues intermediate;

    avgData.reset();
    intermediate.reset();

    avgData.merge(0, intermediate);

    BOOST_TEST(std::ranges::all_of(avgData.hourly, [&](auto val) { return all_close(val, 0.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.daily, [&](auto val) { return all_close(val, 0.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.weekly, [&](auto val) { return all_close(val, 0.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.monthly, [&](auto val) { return all_close(val, 0.0); }));
    BOOST_TEST(all_close(avgData.year, 0.0));
}

BOOST_AUTO_TEST_CASE(Merge_ratioOfOne)
{
    R::AllYears::AverageData avgData(1);
    IntermediateValues intermediate;

    avgData.reset();
    intermediate.reset();

    std::fill_n(intermediate.hour, HOURS_PER_YEAR, 2.0);
    std::fill_n(intermediate.day, DAYS_PER_YEAR, 2.0);
    std::fill_n(intermediate.week, WEEKS_PER_YEAR, 2.0);
    std::fill_n(intermediate.month, MONTHS_PER_YEAR, 2.0);
    intermediate.year = 2.0;

    avgData.merge(0, intermediate);

    BOOST_TEST(std::ranges::all_of(avgData.hourly, [&](auto val) { return all_close(val, 2.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.daily, [&](auto val) { return all_close(val, 2.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.weekly, [&](auto val) { return all_close(val, 2.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.monthly, [&](auto val) { return all_close(val, 2.0); }));
    BOOST_TEST(all_close(avgData.year, 2.0));
}

BOOST_AUTO_TEST_CASE(Merge_ratioOfTwo)
{
    R::AllYears::AverageData avgData(2);
    IntermediateValues intermediate;

    avgData.reset();
    intermediate.reset();

    std::fill_n(intermediate.hour, HOURS_PER_YEAR, 2.0);
    std::fill_n(intermediate.day, DAYS_PER_YEAR, 2.0);
    std::fill_n(intermediate.week, WEEKS_PER_YEAR, 2.0);
    std::fill_n(intermediate.month, MONTHS_PER_YEAR, 2.0);
    intermediate.year = 2.0;

    avgData.merge(1, intermediate);

    BOOST_TEST(std::ranges::all_of(avgData.hourly, [&](auto val) { return all_close(val, 1.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.daily, [&](auto val) { return all_close(val, 1.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.weekly, [&](auto val) { return all_close(val, 1.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.monthly, [&](auto val) { return all_close(val, 1.0); }));
    BOOST_TEST(all_close(avgData.year, 1.0));
}

BOOST_AUTO_TEST_CASE(Merge_several_Intermediates)
{
    R::AllYears::AverageData avgData(3);
    IntermediateValues intermediate;

    avgData.reset();
    intermediate.reset();

    std::fill_n(intermediate.hour, HOURS_PER_YEAR, 1.0);
    std::fill_n(intermediate.day, DAYS_PER_YEAR, 2.0);
    std::fill_n(intermediate.week, WEEKS_PER_YEAR, 3.0);
    std::fill_n(intermediate.month, MONTHS_PER_YEAR, 4.0);
    intermediate.year = 5.0;

    avgData.merge(0, intermediate);

    std::fill_n(intermediate.hour, HOURS_PER_YEAR, 5.0);
    std::fill_n(intermediate.day, DAYS_PER_YEAR, 4.0);
    std::fill_n(intermediate.week, WEEKS_PER_YEAR, 3.0);
    std::fill_n(intermediate.month, MONTHS_PER_YEAR, 2.0);
    intermediate.year = 1.0;

    avgData.merge(1, intermediate);

    std::fill_n(intermediate.hour, HOURS_PER_YEAR, 3.0);
    std::fill_n(intermediate.day, DAYS_PER_YEAR, 3.0);
    std::fill_n(intermediate.week, WEEKS_PER_YEAR, 3.0);
    std::fill_n(intermediate.month, MONTHS_PER_YEAR, 3.0);
    intermediate.year = 3.0;

    avgData.merge(2, intermediate);

    BOOST_TEST(std::ranges::all_of(avgData.hourly, [&](auto val) { return all_close(val, 3.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.daily, [&](auto val) { return all_close(val, 3.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.weekly, [&](auto val) { return all_close(val, 3.0); }));
    BOOST_TEST(std::ranges::all_of(avgData.monthly, [&](auto val) { return all_close(val, 3.0); }));
    BOOST_TEST(all_close(avgData.year, 3.0));
}

BOOST_AUTO_TEST_SUITE_END()
