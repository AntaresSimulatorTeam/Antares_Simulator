/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE "test time series"
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <antares/series/series.h>

using namespace Antares::Data;

// =================
// The fixture
// =================
struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture() : ts(tsnum)
    {
        ts.reset(1, HOURS_PER_YEAR);
        tsnum.resize(1, 1);
        tsnum[0][0] = 0;
    }
    TimeSeries ts;
    TimeSeries::numbers tsnum;
    std::string folder;

    void fillColumn(unsigned int idx);
    void fillColumnReverse(unsigned int idx);

    void fillTsnum();

};

void Fixture::fillColumn(unsigned int idx)
{
    for (unsigned int i = 0; i < ts.timeSeries.height; i++)
        ts.timeSeries[idx][i] = i;
}

void Fixture::fillColumnReverse(unsigned int idx)
{
    for (unsigned int i = 0; i < ts.timeSeries.height; i++)
        ts.timeSeries[idx][i] = HOURS_PER_YEAR - i;
}

void Fixture::fillTsnum()
{
    tsnum.resize(1, ts.timeSeries.width);
    for (unsigned int i = 0; i < ts.timeSeries.width; i++)
        tsnum[0][i] = i;
}

// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(timeseries_tests)

BOOST_FIXTURE_TEST_CASE(getSeriesIndex, Fixture)
{
    tsnum.resize(1, 10);
    for (unsigned int i = 0; i < 10; i++)
        tsnum[0][i] = i;

    ts.resize(2, HOURS_PER_YEAR);
    for (unsigned int i = 0; i < 10; i++)
        BOOST_CHECK_EQUAL(ts.getSeriesIndex(i), i);
}

BOOST_FIXTURE_TEST_CASE(getCoefficientWidth1, Fixture)
{
    fillColumn(0);
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 12), 12);
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 8750), 8750);
}

BOOST_FIXTURE_TEST_CASE(getCoefficientNotInitialized, Fixture)
{
    ts.resize(4, HOURS_PER_YEAR);
    fillTsnum();
    BOOST_CHECK_EQUAL(ts.getCoefficient(3, 12), 0);
    BOOST_CHECK_EQUAL(ts.getCoefficient(3, 8750), 0);
}

BOOST_FIXTURE_TEST_CASE(getCoefficientWidthMoreThan1, Fixture)
{
    ts.resize(5, HOURS_PER_YEAR);
    fillTsnum();

    fillColumn(3);
    BOOST_CHECK_EQUAL(ts.getCoefficient(3, 12), 12);
    BOOST_CHECK_EQUAL(ts.getCoefficient(3, 4858), 4858);

    fillColumnReverse(2);
    BOOST_CHECK_EQUAL(ts.getCoefficient(2, 20), 8740);
    BOOST_CHECK_EQUAL(ts.getCoefficient(2, 4567), HOURS_PER_YEAR - 4567);
}

BOOST_FIXTURE_TEST_CASE(getColumn, Fixture)
{
    auto col = ts.getColumn(0);
    BOOST_CHECK_EQUAL(col[38], 0);
    BOOST_CHECK_EQUAL(col[7463], 0);

    ts.resize(4, HOURS_PER_YEAR);
    fillTsnum();
    fillColumn(2);

    col = ts.getColumn(2);
    BOOST_CHECK_EQUAL(col[38], 38);
    BOOST_CHECK_EQUAL(col[7463], 7463);
}

BOOST_FIXTURE_TEST_CASE(operatorArray, Fixture)
{
    ts.resize(4, HOURS_PER_YEAR);
    fillTsnum();
    auto* col = ts[2];
    col[27] = 12;
    BOOST_CHECK_EQUAL(ts.getCoefficient(2, 27), 12);
}

BOOST_FIXTURE_TEST_CASE(getCoefficientSpecificData, Fixture)
{
    ts.resize(2, 2);
    fillTsnum();
    tsnum[0][0] = 1;
    tsnum[0][1] = 0;
    ts.timeSeries[0][0] = 12.5;
    ts.timeSeries[0][1] = 74.74;
    ts.timeSeries[1][0] = -57;
    ts.timeSeries[1][1] = 29;

    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 1), 74.74);
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 0), -57);
    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 0), 12.5);
}

BOOST_AUTO_TEST_SUITE_END()
