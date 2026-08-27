// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro dataseries

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>

#include <boost/test/unit_test.hpp>

#include <antares/study/parts/hydro/series.h>

using namespace Antares::Data;
namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// These tests exercise DataSeriesHydro (series.cpp), which a coverage run
// showed was almost entirely untested: only the constructor/reset()/resizeTS()
// (exercised indirectly through PartHydro) and a partial (success-path only)
// LoadMaxPower() were ever executed.
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(hydro_dataseries)

BOOST_AUTO_TEST_CASE(saveToFolder_then_load_roundtrip_hourly)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    DataSeriesHydro writer;
    writer.reset();
    writer.ror.fill(3.0);
    writer.storage.fill(4.0);
    writer.mingen.fill(5.0);
    writer.maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    writer.maxHourlyGenPower.fill(6.0);
    writer.maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
    writer.maxHourlyPumpPower.fill(7.0);

    BOOST_REQUIRE(writer.saveToFolder("area1",
                                      dir.string(),
                                      Parameters::Compatibility::HydroPmax::Hourly));

    DataSeriesHydro reader;
    BOOST_CHECK(reader.loadGenerationTS("area1", dir, StudyVersion(8, 6)));
    BOOST_CHECK_CLOSE(reader.ror.timeSeries[0][0], 3.0, 0.0001);
    BOOST_CHECK_CLOSE(reader.storage.timeSeries[0][0], 4.0, 0.0001);
    BOOST_CHECK_CLOSE(reader.mingen.timeSeries[0][0], 5.0, 0.0001);

    BOOST_CHECK(reader.LoadMaxPower("area1", dir));
    BOOST_CHECK_CLOSE(reader.maxHourlyGenPower.timeSeries[0][0], 6.0, 0.0001);
    BOOST_CHECK_CLOSE(reader.maxHourlyPumpPower.timeSeries[0][0], 7.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(saveToFolder_daily_does_not_write_maxpower_files)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    DataSeriesHydro writer;
    writer.reset();

    BOOST_REQUIRE(writer.saveToFolder("area1",
                                      dir.string(),
                                      Parameters::Compatibility::HydroPmax::Daily));

    BOOST_CHECK(fs::exists(dir / "area1" / "ror.txt"));
    BOOST_CHECK(fs::exists(dir / "area1" / "mod.txt"));
    BOOST_CHECK(fs::exists(dir / "area1" / "mingen.txt"));
    BOOST_CHECK(!fs::exists(dir / "area1" / "maxHourlyGenPower.txt"));
    BOOST_CHECK(!fs::exists(dir / "area1" / "maxHourlyPumpPower.txt"));

    DataSeriesHydro reader;
    BOOST_CHECK(!reader.LoadMaxPower("area1", dir));
}

BOOST_AUTO_TEST_CASE(loadGenerationTS_before_8_6_skips_mingen)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    DataSeriesHydro writer;
    writer.reset();
    writer.ror.fill(1.0);
    writer.storage.fill(2.0);
    writer.mingen.fill(9.0);
    BOOST_REQUIRE(writer.saveToFolder("area1",
                                      dir.string(),
                                      Parameters::Compatibility::HydroPmax::Daily));

    DataSeriesHydro reader;
    BOOST_CHECK(reader.loadGenerationTS("area1", dir, StudyVersion(8, 5)));
    BOOST_CHECK_CLOSE(reader.ror.timeSeries[0][0], 1.0, 0.0001);
    BOOST_CHECK_CLOSE(reader.storage.timeSeries[0][0], 2.0, 0.0001);
    // mingen.txt exists on disk but must not be read for a study version < 8.6
    BOOST_CHECK_CLOSE(reader.mingen.timeSeries[0][0], 0.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(loadGenerationTS_missing_folder_returns_false)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    DataSeriesHydro reader;
    BOOST_CHECK(!reader.loadGenerationTS("no_such_area", dir, StudyVersion(8, 6)));
}

BOOST_AUTO_TEST_CASE(LoadMaxPower_missing_folder_returns_false)
{
    auto dir = CREATE_TMP_DIR_BASED_ON_TEST_NAME();

    DataSeriesHydro reader;
    BOOST_CHECK(!reader.LoadMaxPower("no_such_area", dir));
}

BOOST_AUTO_TEST_CASE(copyGenerationTS_copies_ror_storage_mingen_only)
{
    DataSeriesHydro src;
    src.reset();
    src.ror.fill(1.0);
    src.storage.fill(2.0);
    src.mingen.fill(3.0);
    src.maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    src.maxHourlyGenPower.fill(4.0);

    DataSeriesHydro dst;
    dst.copyGenerationTS(src);

    BOOST_CHECK_CLOSE(dst.ror.timeSeries[0][0], 1.0, 0.0001);
    BOOST_CHECK_CLOSE(dst.storage.timeSeries[0][0], 2.0, 0.0001);
    BOOST_CHECK_CLOSE(dst.mingen.timeSeries[0][0], 3.0, 0.0001);
    // maxHourlyGenPower is not part of the "generation" TS copied here
    BOOST_CHECK_CLOSE(dst.maxHourlyGenPower.timeSeries[0][0], 0.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(getDailyMaxGenAndPumpPowerFromHourlyTS_averages_per_day)
{
    DataSeriesHydro d;
    d.maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    d.maxHourlyGenPower.fill(10.0);
    d.maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
    d.maxHourlyPumpPower.fill(20.0);

    Antares::Matrix<> dailyGen = d.getDailyMaxGenPowerFromHourlyTS();
    Antares::Matrix<> dailyPump = d.getDailyMaxPumpPowerFromHourlyTS();

    BOOST_REQUIRE_EQUAL(dailyGen.width, 1u);
    BOOST_REQUIRE_EQUAL(dailyGen.height, DAYS_PER_YEAR);
    BOOST_CHECK_CLOSE(dailyGen[0][0], 10.0, 0.0001);
    BOOST_CHECK_CLOSE(dailyGen[0][DAYS_PER_YEAR - 1], 10.0, 0.0001);

    BOOST_REQUIRE_EQUAL(dailyPump.width, 1u);
    BOOST_REQUIRE_EQUAL(dailyPump.height, DAYS_PER_YEAR);
    BOOST_CHECK_CLOSE(dailyPump[0][0], 20.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(TScount_returns_max_column_count_across_series)
{
    DataSeriesHydro d;
    d.storage.resize(3, DAYS_PER_YEAR);
    d.ror.resize(1, HOURS_PER_YEAR);
    BOOST_CHECK_EQUAL(d.TScount(), 3u);
}

BOOST_AUTO_TEST_CASE(resizeTSinDeratedMode_not_derated_is_noop)
{
    DataSeriesHydro d;
    d.ror.resize(3, HOURS_PER_YEAR);
    d.resizeTSinDeratedMode(false, StudyVersion(8, 6), Parameters::Compatibility::HydroPmax::Hourly);
    BOOST_CHECK_EQUAL(d.ror.numberOfColumns(), 3u);
}

BOOST_AUTO_TEST_CASE(resizeTSinDeratedMode_pre_8_6_skips_mingen_and_maxpower)
{
    DataSeriesHydro d;
    d.ror.resize(2, HOURS_PER_YEAR);
    d.storage.resize(2, DAYS_PER_YEAR);
    d.mingen.resize(2, HOURS_PER_YEAR);
    d.maxHourlyGenPower.resize(2, HOURS_PER_YEAR);
    d.maxHourlyPumpPower.resize(2, HOURS_PER_YEAR);

    d.resizeTSinDeratedMode(true, StudyVersion(8, 5), Parameters::Compatibility::HydroPmax::Hourly);

    BOOST_CHECK_EQUAL(d.ror.numberOfColumns(), 1u);
    BOOST_CHECK_EQUAL(d.storage.numberOfColumns(), 1u);
    BOOST_CHECK_EQUAL(d.mingen.numberOfColumns(), 2u); // untouched: below v8.6
    BOOST_CHECK_EQUAL(d.maxHourlyGenPower.numberOfColumns(), 2u); // gated by the same check
    BOOST_CHECK_EQUAL(d.maxHourlyPumpPower.numberOfColumns(), 2u);
}

BOOST_AUTO_TEST_CASE(resizeTSinDeratedMode_post_8_6_daily_skips_maxpower)
{
    DataSeriesHydro d;
    d.mingen.resize(2, HOURS_PER_YEAR);
    d.maxHourlyGenPower.resize(2, HOURS_PER_YEAR);
    d.maxHourlyPumpPower.resize(2, HOURS_PER_YEAR);

    d.resizeTSinDeratedMode(true, StudyVersion(8, 6), Parameters::Compatibility::HydroPmax::Daily);

    BOOST_CHECK_EQUAL(d.mingen.numberOfColumns(), 1u);
    BOOST_CHECK_EQUAL(d.maxHourlyGenPower.numberOfColumns(), 2u); // untouched: hydroPmax != Hourly
    BOOST_CHECK_EQUAL(d.maxHourlyPumpPower.numberOfColumns(), 2u);
}

BOOST_AUTO_TEST_CASE(resizeTSinDeratedMode_post_8_6_hourly_averages_everything)
{
    DataSeriesHydro d;
    d.mingen.resize(2, HOURS_PER_YEAR);
    d.maxHourlyGenPower.resize(2, HOURS_PER_YEAR);
    d.maxHourlyPumpPower.resize(2, HOURS_PER_YEAR);

    d.resizeTSinDeratedMode(true, StudyVersion(8, 6), Parameters::Compatibility::HydroPmax::Hourly);

    BOOST_CHECK_EQUAL(d.mingen.numberOfColumns(), 1u);
    BOOST_CHECK_EQUAL(d.maxHourlyGenPower.numberOfColumns(), 1u);
    BOOST_CHECK_EQUAL(d.maxHourlyPumpPower.numberOfColumns(), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
