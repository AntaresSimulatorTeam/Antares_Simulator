// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test time series"

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <fstream>
#include <memory>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/series/series.h>
#include <antares/solver/simulation/timeseries-numbers.h>

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

    Fixture():
        ts(tsnum)
    {
        ts.reset(1, HOURS_PER_YEAR);
        tsnum.reset(1);
        tsnum[0] = 0;
    }

    TimeSeriesNumbers tsnum;
    TimeSeries ts;
    std::string folder;

    void fillColumn(unsigned int idx);
    void fillColumnReverse(unsigned int idx);

    void fillTsnum();
};

void Fixture::fillColumn(unsigned int idx)
{
    for (unsigned int i = 0; i < ts.timeSeries.height(); i++)
    {
        ts.timeSeries[idx][i] = i;
    }
}

void Fixture::fillColumnReverse(unsigned int idx)
{
    for (unsigned int i = 0; i < ts.timeSeries.height(); i++)
    {
        ts.timeSeries[idx][i] = HOURS_PER_YEAR - i;
    }
}

void Fixture::fillTsnum()
{
    tsnum.reset(ts.timeSeries.width());
    for (unsigned int i = 0; i < ts.timeSeries.width(); i++)
    {
        tsnum[i] = i;
    }
}

class FixtureMultipleTS
{
public:
    void init(const std::vector<int>& width)
    {
        const int height = 10; // Arbitrary
        ts.resize(width.size());
        for (size_t idx = 0; int w: width)
        {
            // TimeSeries::TimeSeries does not exist, so we use pointers
            ts[idx] = std::make_unique<TimeSeries>(tsnum); // ts[idx] is registered to tsnum here
            tsnum.registerSeries(ts[idx].get(), std::to_string(idx));
            ts[idx]->reset(w, height);
            idx++;
        }
    }

public:
    TimeSeriesNumbers tsnum;

private:
    std::vector<std::unique_ptr<TimeSeries>> ts;
};

// ==================
// Tests section
// ==================

BOOST_AUTO_TEST_SUITE(timeseries_tests)

BOOST_FIXTURE_TEST_CASE(averaging_ignores_trailing_tab_padding, Fixture)
{
    const auto directory = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    const auto path = directory / "series.txt";
    {
        std::ofstream output(path);
        BOOST_REQUIRE(output);
        for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
        {
            output << "12\t\n";
        }
    }
    BOOST_REQUIRE(ts.loadFromFile(path, true));
    BOOST_REQUIRE_EQUAL(ts.timeSeries.width(), 1);
    BOOST_CHECK_EQUAL(ts.timeSeries[0][0], 12.);
    BOOST_CHECK_EQUAL(ts.timeSeries[0][HOURS_PER_YEAR - 1], 12.);
}

BOOST_FIXTURE_TEST_CASE(load_hourly_prefix_and_reject_invalid_required_rows, Fixture)
{
    const auto directory = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    const auto path = directory / "series.txt";
    {
        std::ofstream output(path);
        for (unsigned int i = 0; i < HOURS_PER_YEAR; ++i)
        {
            output << i << '\n';
        }
        output << "ignored\n";
    }
    BOOST_REQUIRE(ts.loadFromFile(path, false));
    BOOST_CHECK_EQUAL(ts.timeSeries.height(), HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(ts.timeSeries[0][HOURS_PER_YEAR - 1], HOURS_PER_YEAR - 1);
    const auto checkReset = [&]()
    {
        BOOST_REQUIRE_EQUAL(ts.timeSeries.width(), 1);
        BOOST_REQUIRE_EQUAL(ts.timeSeries.height(), HOURS_PER_YEAR);
        BOOST_CHECK(ts.timeSeries.containsOnlyZero());
    };
    {
        std::ofstream output(path);
        output << "bad\n";
        for (unsigned int i = 1; i < HOURS_PER_YEAR; ++i)
        {
            output << i << '\n';
        }
    }
    BOOST_CHECK(!ts.loadFromFile(path, false));
    checkReset();
    ts.reset(2, HOURS_PER_YEAR);
    ts.timeSeries.fill(42.);
    {
        std::ofstream output(path);
        for (unsigned int i = 0; i < HOURS_PER_YEAR - 1; ++i)
        {
            output << i << '\n';
        }
    }
    BOOST_CHECK(!ts.loadFromFile(path, false));
    checkReset();
    ts.reset(2, HOURS_PER_YEAR);
    ts.timeSeries.fill(42.);
    BOOST_CHECK(!ts.loadFromFile(directory / "missing.txt", true));
    checkReset();
}

BOOST_FIXTURE_TEST_CASE(getSeriesIndex, Fixture)
{
    tsnum.reset(10);
    for (unsigned int i = 0; i < 10; i++)
    {
        tsnum[i] = i;
    }

    ts.resize(2, HOURS_PER_YEAR);
    for (unsigned int i = 0; i < 10; i++)
    {
        BOOST_CHECK_EQUAL(ts.getSeriesIndex(i), i);
    }
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
    auto col = ts[2];
    col[27] = 12;
    BOOST_CHECK_EQUAL(ts.getCoefficient(2, 27), 12);
}

BOOST_FIXTURE_TEST_CASE(getCoefficientSpecificData, Fixture)
{
    ts.resize(2, 2);
    fillTsnum();
    tsnum[0] = 1;
    tsnum[1] = 0;
    ts.timeSeries[0][0] = 12.5;
    ts.timeSeries[0][1] = 74.74;
    ts.timeSeries[1][0] = -57;
    ts.timeSeries[1][1] = 29;

    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 1), 74.74);
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 0), -57);
    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 0), 12.5);
}

// SINGLE COLUMN
BOOST_FIXTURE_TEST_CASE(getCoefficient_SingleColumn, Fixture)
{
    ts.resize(1, 2);

    // Here, we provide 2 time series numbers...
    tsnum.reset(2);
    for (unsigned int i = 0; i < 2; i++)
    {
        tsnum[i] = i;
    }

    // ...but only one column
    ts.timeSeries[0][0] = 12.5;
    ts.timeSeries[0][1] = 74.74;

    // year=1
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 0), 12.5);
    BOOST_CHECK_EQUAL(ts.getCoefficient(0, 1), 74.74);

    // year=2
    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 0), 12.5);
    BOOST_CHECK_EQUAL(ts.getCoefficient(1, 1), 74.74);
}

// VALID CONFIGURATIONS
BOOST_FIXTURE_TEST_CASE(checkSizeOK_1TS, FixtureMultipleTS)
{
    init({11});
    BOOST_CHECK(!tsnum.checkSeriesNumberOfColumnsConsistency());
}

BOOST_FIXTURE_TEST_CASE(checkSizeOK_2TS, FixtureMultipleTS)
{
    init({12, 12});
    BOOST_CHECK(!tsnum.checkSeriesNumberOfColumnsConsistency());
}

BOOST_FIXTURE_TEST_CASE(checkSizeOK_4TS, FixtureMultipleTS)
{
    init({22, 22, 1, 22});
    BOOST_CHECK(!tsnum.checkSeriesNumberOfColumnsConsistency());
}

BOOST_FIXTURE_TEST_CASE(checkSizeKO_2TS, FixtureMultipleTS)
{
    init({11, 12});
    BOOST_CHECK(tsnum.checkSeriesNumberOfColumnsConsistency());
}

BOOST_FIXTURE_TEST_CASE(checkSizeKO_4TS, FixtureMultipleTS)
{
    init({22, 22, 1, 21});
    BOOST_CHECK(tsnum.checkSeriesNumberOfColumnsConsistency());
}

BOOST_AUTO_TEST_SUITE_END()
