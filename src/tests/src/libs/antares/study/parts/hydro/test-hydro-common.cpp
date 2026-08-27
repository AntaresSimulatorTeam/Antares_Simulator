// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro common

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <antares/study/parts/hydro/series.h>
#include <antares/study/study.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

// =================
// The fixture
// =================
struct CommonFixture
{
    std::filesystem::path path;
    StudyLoadOptions options;
    StudyVersion version = StudyVersion::latest();

    CommonFixture():
        tmp(createTempDirectory(boost::unit_test::framework::current_test_case().p_name)),
        hydroIni(tmp / "hydro.ini"),
        study(std::make_unique<Study>())
    {
        east = new Area("east"); // freed by ~AreaList
        study->areas.add(east);
        west = new Area("west"); // freed by ~AreaList
        study->areas.add(west);
    }

    bool load()
    {
        // LoadIniFile is static and iterates over all areas in the study,
        // so a single call covers both east and west.
        return PartHydro::LoadIniFile(*study, tmp);
    }

    bool validate()
    {
        return PartHydro::validate(*study);
    }

    bool loadFromFolder()
    {
        return PartHydro::LoadFromFolder(*study, tmp);
    }

    bool saveToFolder(Parameters::Compatibility::HydroPmax hydroPmax)
    {
        return PartHydro::SaveToFolder(study->areas, tmp.string(), hydroPmax);
    }

    // common/capacity is where credit modulations/inflow pattern/water values/max energy
    // CSV files are read from and written to; it is not created automatically.
    fs::path capacityFolder() const
    {
        fs::path dir = tmp / "common" / "capacity";
        fs::create_directories(dir);
        return dir;
    }

    Study& studyRef()
    {
        return *study;
    }

    fs::path folder() const
    {
        return tmp;
    }

    ~CommonFixture()
    {
        std::error_code ec;
        fs::remove_all(tmp, ec);
    }

    void writeFile(const std::string& content);
    void writeValidFile();
    void writeInvalidFile();

public:
    Area* east;
    Area* west;

private:
    fs::path tmp;
    fs::path hydroIni;
    std::unique_ptr<Study> study;
};

void CommonFixture::writeFile(const std::string& content)
{
    std::ofstream outfile(hydroIni);
    outfile << content;
}

void CommonFixture::writeValidFile()
{
    writeFile(R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000

[reservoir]
east = true)");
}

void CommonFixture::writeInvalidFile()
{
    writeFile(R"([overflow spilled cost difference]
east = 1.00000
west = 2.31000
wrongarea = 1.414

[reservoir]
east = true)");
}

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(test_read_valid_file, CommonFixture)
{
    writeValidFile();
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.overflowSpilledCostDifference, 1.0);
    BOOST_CHECK_EQUAL(west->hydro.overflowSpilledCostDifference, 2.31000);
}

BOOST_FIXTURE_TEST_CASE(test_read_invalid_file, CommonFixture)
{
    writeInvalidFile();
    BOOST_CHECK(!load());
}

BOOST_FIXTURE_TEST_CASE(test_missing_file_returns_false, CommonFixture)
{
    // No file written — ini.open() must fail
    BOOST_CHECK(!load());
}

BOOST_FIXTURE_TEST_CASE(test_empty_file_returns_true, CommonFixture)
{
    writeFile("");
    BOOST_CHECK(load());
}

BOOST_FIXTURE_TEST_CASE(test_inter_daily_breakdown, CommonFixture)
{
    writeFile("[inter-daily-breakdown]\neast = 0.5\nwest = 0.75\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
    BOOST_CHECK_EQUAL(west->hydro.interDailyBreakdown, 0.75);
}

BOOST_FIXTURE_TEST_CASE(test_intra_daily_modulation, CommonFixture)
{
    writeFile("[intra-daily-modulation]\neast = 12.0\nwest = 6.0\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.intraDailyModulation, 12.0);
    BOOST_CHECK_EQUAL(west->hydro.intraDailyModulation, 6.0);
}

BOOST_FIXTURE_TEST_CASE(test_inter_monthly_breakdown, CommonFixture)
{
    writeFile("[inter-monthly-breakdown]\neast = 0.3\nwest = 0.9\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.intermonthlyBreakdown, 0.3);
    BOOST_CHECK_EQUAL(west->hydro.intermonthlyBreakdown, 0.9);
}

BOOST_FIXTURE_TEST_CASE(test_reservoir_management, CommonFixture)
{
    writeFile("[reservoir]\neast = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.reservoirManagement, true);
    BOOST_CHECK_EQUAL(west->hydro.reservoirManagement, false);
}

BOOST_FIXTURE_TEST_CASE(test_reservoir_capacity, CommonFixture)
{
    writeFile("[reservoir capacity]\neast = 1500.0\nwest = 750.5\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.reservoirCapacity, 1500.0);
    BOOST_CHECK_EQUAL(west->hydro.reservoirCapacity, 750.5);
}

BOOST_FIXTURE_TEST_CASE(test_follow_load_modulations, CommonFixture)
{
    writeFile("[follow load]\neast = false\nwest = false\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.followLoadModulations, false);
    BOOST_CHECK_EQUAL(west->hydro.followLoadModulations, false);
}

BOOST_FIXTURE_TEST_CASE(test_use_water_value, CommonFixture)
{
    writeFile("[use water]\neast = true\nwest = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.useWaterValue, true);
    BOOST_CHECK_EQUAL(west->hydro.useWaterValue, true);
}

BOOST_FIXTURE_TEST_CASE(test_hard_bounds_on_rule_curves, CommonFixture)
{
    writeFile("[hard bounds]\neast = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.hardBoundsOnRuleCurves, true);
    BOOST_CHECK_EQUAL(west->hydro.hardBoundsOnRuleCurves, false);
}

BOOST_FIXTURE_TEST_CASE(test_use_heuristic_target, CommonFixture)
{
    writeFile("[use heuristic]\neast = false\nwest = false\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.useHeuristicTarget, false);
    BOOST_CHECK_EQUAL(west->hydro.useHeuristicTarget, false);
}

BOOST_FIXTURE_TEST_CASE(test_power_to_level, CommonFixture)
{
    writeFile("[power to level]\neast = true\nwest = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.powerToLevel, true);
    BOOST_CHECK_EQUAL(west->hydro.powerToLevel, true);
}

BOOST_FIXTURE_TEST_CASE(test_initialize_reservoir_level_date, CommonFixture)
{
    writeFile("[initialize reservoir date]\neast = 5\nwest = 11\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.initializeReservoirLevelDate, 5);
    BOOST_CHECK_EQUAL(west->hydro.initializeReservoirLevelDate, 11);
}

BOOST_FIXTURE_TEST_CASE(test_use_leeway, CommonFixture)
{
    writeFile("[use leeway]\neast = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.useLeeway, true);
    BOOST_CHECK_EQUAL(west->hydro.useLeeway, false);
}

BOOST_FIXTURE_TEST_CASE(test_leeway_lower_bound, CommonFixture)
{
    writeFile("[leeway low]\neast = 0.2\nwest = 0.4\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 0.2);
    BOOST_CHECK_EQUAL(west->hydro.leewayLowerBound, 0.4);
}

BOOST_FIXTURE_TEST_CASE(test_leeway_upper_bound, CommonFixture)
{
    writeFile("[leeway up]\neast = 0.8\nwest = 0.6\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 0.8);
    BOOST_CHECK_EQUAL(west->hydro.leewayUpperBound, 0.6);
}

BOOST_FIXTURE_TEST_CASE(test_pumping_efficiency, CommonFixture)
{
    writeFile("[pumping efficiency]\neast = 0.85\nwest = 0.9\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.pumpingEfficiency, 0.85);
    BOOST_CHECK_EQUAL(west->hydro.pumpingEfficiency, 0.9);
}

BOOST_FIXTURE_TEST_CASE(test_unknown_area_returns_false, CommonFixture)
{
    writeFile("[inter-daily-breakdown]\neast = 0.5\nunknown = 0.3\n");
    BOOST_CHECK(!load());
    // Known area was written before the unknown one, so it is still loaded
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
}

BOOST_FIXTURE_TEST_CASE(test_empty_section_returns_false, CommonFixture)
{
    // Section header present but no key=value pairs — firstProperty is null
    writeFile("[inter-daily-breakdown]\n");
    BOOST_CHECK(!load());
}

BOOST_FIXTURE_TEST_CASE(test_case_insensitive_area_name, CommonFixture)
{
    // Area IDs are stored as lowercase; keys from the INI file are also lowercased before lookup
    writeFile("[inter-daily-breakdown]\nEast = 0.5\nWEST = 0.75\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
    BOOST_CHECK_EQUAL(west->hydro.interDailyBreakdown, 0.75);
}

BOOST_FIXTURE_TEST_CASE(test_absent_section_preserves_constructor_default, CommonFixture)
{
    // Only write one section; other properties must keep their PartHydro() defaults
    writeFile("[reservoir]\neast = true\n");
    BOOST_CHECK(load());
    BOOST_CHECK_EQUAL(east->hydro.reservoirManagement, true);
    // Constructor defaults for untouched fields
    BOOST_CHECK_EQUAL(east->hydro.overflowSpilledCostDifference, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.followLoadModulations, true);
    BOOST_CHECK_EQUAL(east->hydro.useWaterValue, false);
    BOOST_CHECK_EQUAL(east->hydro.pumpingEfficiency, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 1.0);
}

// ---------------------------------------------------------------------------
// The following tests exercise PartHydro/free-function members of
// container.cpp that a coverage run showed were never executed by the rest
// of the test suite (LoadIniFile/loadProperties/reset are already covered
// above and via other fixtures, and are intentionally not retested here).
// ---------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_copyFrom_copies_all_fields)
{
    PartHydro src;
    src.reset();
    src.interDailyBreakdown = 0.42;
    src.intraDailyModulation = 3.5;
    src.intermonthlyBreakdown = 0.7;
    src.reservoirManagement = true;
    src.reservoirCapacity = 1234.5;
    src.followLoadModulations = false;
    src.useWaterValue = true;
    src.hardBoundsOnRuleCurves = true;
    src.useHeuristicTarget = false;
    src.initializeReservoirLevelDate = 5;
    src.useLeeway = true;
    src.powerToLevel = true;
    src.leewayLowerBound = 0.2;
    src.leewayUpperBound = 0.8;
    src.pumpingEfficiency = 0.65;
    src.creditModulation[0][0] = 42.0;
    src.inflowPattern[0][0] = 7.0;
    src.waterValues[0][0] = 3.0;
    src.dailyNbHoursAtGenPmax[0][0] = 12.0;
    src.dailyNbHoursAtPumpPmax[0][0] = 6.0;

    PartHydro dst;
    dst.copyFrom(src);

    BOOST_CHECK_EQUAL(dst.interDailyBreakdown, 0.42);
    BOOST_CHECK_EQUAL(dst.intraDailyModulation, 3.5);
    BOOST_CHECK_EQUAL(dst.intermonthlyBreakdown, 0.7);
    BOOST_CHECK_EQUAL(dst.reservoirManagement, true);
    BOOST_CHECK_EQUAL(dst.reservoirCapacity, 1234.5);
    BOOST_CHECK_EQUAL(dst.followLoadModulations, false);
    BOOST_CHECK_EQUAL(dst.useWaterValue, true);
    BOOST_CHECK_EQUAL(dst.hardBoundsOnRuleCurves, true);
    BOOST_CHECK_EQUAL(dst.useHeuristicTarget, false);
    BOOST_CHECK_EQUAL(dst.initializeReservoirLevelDate, 5);
    BOOST_CHECK_EQUAL(dst.useLeeway, true);
    BOOST_CHECK_EQUAL(dst.powerToLevel, true);
    BOOST_CHECK_EQUAL(dst.leewayLowerBound, 0.2);
    BOOST_CHECK_EQUAL(dst.leewayUpperBound, 0.8);
    BOOST_CHECK_EQUAL(dst.pumpingEfficiency, 0.65);
    BOOST_CHECK_EQUAL(dst.creditModulation[0][0], 42.0);
    BOOST_CHECK_EQUAL(dst.inflowPattern[0][0], 7.0);
    BOOST_CHECK_EQUAL(dst.waterValues[0][0], 3.0);
    BOOST_CHECK_EQUAL(dst.dailyNbHoursAtGenPmax[0][0], 12.0);
    BOOST_CHECK_EQUAL(dst.dailyNbHoursAtPumpPmax[0][0], 6.0);
}

BOOST_AUTO_TEST_CASE(test_CheckDailyMaxEnergy_valid_and_invalid)
{
    PartHydro hydro;
    hydro.reset(); // sizes and fills dailyNbHoursAt{Gen,Pump}Pmax with valid 24h values

    BOOST_CHECK(hydro.CheckDailyMaxEnergy("some_area"));

    hydro.dailyNbHoursAtGenPmax[0][10] = 25.0; // > 24 : invalid
    BOOST_CHECK(!hydro.CheckDailyMaxEnergy("some_area"));

    hydro.reset();
    hydro.dailyNbHoursAtPumpPmax[0][20] = -1.0; // < 0 : invalid
    BOOST_CHECK(!hydro.CheckDailyMaxEnergy("some_area"));
}

BOOST_FIXTURE_TEST_CASE(test_LoadDailyMaxEnergy_roundtrip, CommonFixture)
{
    fs::path capacity = capacityFolder();

    PartHydro writer;
    writer.reset();
    writer.dailyNbHoursAtGenPmax.fillColumn(0, 12.5);
    writer.dailyNbHoursAtPumpPmax.fillColumn(0, 8.5);
    BOOST_REQUIRE(writer.dailyNbHoursAtGenPmax
                    .saveToCSVFile((capacity / "maxDailyGenEnergy_area1.txt").string(), 2));
    BOOST_REQUIRE(writer.dailyNbHoursAtPumpPmax
                    .saveToCSVFile((capacity / "maxDailyPumpEnergy_area1.txt").string(), 2));

    PartHydro reader;
    BOOST_CHECK(reader.LoadDailyMaxEnergy(folder(), "area1"));
    BOOST_CHECK_CLOSE(reader.dailyNbHoursAtGenPmax[0][0], 12.5, 0.0001);
    BOOST_CHECK_CLOSE(reader.dailyNbHoursAtPumpPmax[0][0], 8.5, 0.0001);

    PartHydro missing;
    BOOST_CHECK(!missing.LoadDailyMaxEnergy(folder(), "no_such_area"));
}

BOOST_AUTO_TEST_CASE(test_count_reflects_series_TScount)
{
    PartHydro hydro;
    hydro.series = std::make_unique<DataSeriesHydro>();

    // The DataSeriesHydro constructor gives mingen/maxHourlyGenPower/maxHourlyPumpPower a
    // default width of 1, so a freshly constructed series already has TScount() == 1.
    BOOST_CHECK_EQUAL(hydro.count(), 1u);

    hydro.series->mingen.resize(0, 0);
    hydro.series->maxHourlyGenPower.resize(0, 0);
    hydro.series->maxHourlyPumpPower.resize(0, 0);
    // RuleCurves' constructor also defaults max/min/avg to a width of 1
    hydro.series->ruleCurves.max.resize(0, 0);
    hydro.series->ruleCurves.min.resize(0, 0);
    hydro.series->ruleCurves.avg.resize(0, 0);
    BOOST_CHECK_EQUAL(hydro.count(), 0u);

    hydro.series->storage.resize(3, 5);
    BOOST_CHECK_EQUAL(hydro.count(), 1u);
}

BOOST_AUTO_TEST_CASE(test_getWaterValue_normal_and_clamped_levels)
{
    Antares::Matrix<double> waterValues;
    waterValues.reset(101, 1); // 101 levels (0..100), 1 day
    for (unsigned int level = 0; level < 101; ++level)
    {
        waterValues[level][0] = (double)level * 10.0;
    }

    // normal case: level 42.9 -> levelDown = 42
    BOOST_CHECK_CLOSE(getWaterValue(42.9, waterValues, 0), 420.0, 0.0001);

    // negative level clamped to 0
    BOOST_CHECK_CLOSE(getWaterValue(-5.0, waterValues, 0), 0.0, 0.0001);

    // level >= 100 clamped to 99 (the last layer is deliberately not used)
    BOOST_CHECK_CLOSE(getWaterValue(150.0, waterValues, 0), 990.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(test_getWeeklyModulation_exact_and_interpolated_and_clamped)
{
    Antares::Matrix<double, double> creditMod;
    creditMod.reset(101, 2);
    for (unsigned int level = 0; level < 101; ++level)
    {
        creditMod[level][0] = (double)level;
        creditMod[level][1] = (double)level * 2.0;
    }

    // exact integer level: no interpolation
    BOOST_CHECK_CLOSE(getWeeklyModulation(10.0, creditMod, 0), 10.0, 0.0001);

    // fractional level: linear interpolation between floor and ceil
    BOOST_CHECK_CLOSE(getWeeklyModulation(10.25, creditMod, 0), 10.25, 0.0001);

    // negative level: clamped to level 0 (kept within (-1, 0) so the
    // (unclamped) ceil-side index stays in bounds, see getWeeklyModulation)
    BOOST_CHECK_CLOSE(getWeeklyModulation(-0.5, creditMod, 1), 0.0, 0.0001);

    // level above 100: clamped to level 100 (kept within (100, 101) so the
    // (unclamped) floor-side index stays in bounds)
    BOOST_CHECK_CLOSE(getWeeklyModulation(100.5, creditMod, 1), 200.0, 0.0001);
}

BOOST_FIXTURE_TEST_CASE(test_validate_all_defaults_returns_true, CommonFixture)
{
    east->hydro.reset();
    west->hydro.reset();
    BOOST_CHECK(validate());
}

BOOST_FIXTURE_TEST_CASE(test_validate_clamps_invalid_scalar_properties, CommonFixture)
{
    east->hydro.reset();
    west->hydro.reset();

    east->hydro.reservoirManagement = true;
    east->hydro.reservoirCapacity = -5.; // triggers both the "not defined" and "invalid" checks
    east->hydro.useHeuristicTarget = false;
    east->hydro.useWaterValue = false;      // invalid combination: both disabled
    east->hydro.intraDailyModulation = 0.5; // must be >= 1
    east->hydro.intermonthlyBreakdown = -1.;
    east->hydro.initializeReservoirLevelDate = -1;
    east->hydro.leewayLowerBound = -1.;
    east->hydro.leewayUpperBound = -1.;
    east->hydro.pumpingEfficiency = -1.;

    BOOST_CHECK(!validate());

    BOOST_CHECK_EQUAL(east->hydro.reservoirCapacity, 0.);
    BOOST_CHECK_EQUAL(east->hydro.intraDailyModulation, 1.);
    BOOST_CHECK_EQUAL(east->hydro.intermonthlyBreakdown, 0.);
    BOOST_CHECK_EQUAL(east->hydro.initializeReservoirLevelDate, 0);
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 0.);
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 0.);
    BOOST_CHECK_EQUAL(east->hydro.pumpingEfficiency, 0.);

    // west is untouched: still holds reset() defaults
    BOOST_CHECK_EQUAL(west->hydro.intraDailyModulation, 24.);
    BOOST_CHECK_EQUAL(west->hydro.leewayLowerBound, 1.);
    BOOST_CHECK_EQUAL(west->hydro.leewayUpperBound, 1.);
}

BOOST_FIXTURE_TEST_CASE(test_validate_leeway_lower_greater_than_upper, CommonFixture)
{
    east->hydro.reset();
    west->hydro.reset();

    east->hydro.leewayLowerBound = 0.8;
    east->hydro.leewayUpperBound = 0.2;

    // Unlike the other checks, an inverted leeway range is only logged, not
    // reported as a validation failure and not corrected (existing behavior).
    BOOST_CHECK(validate());
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 0.8);
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 0.2);
}

BOOST_FIXTURE_TEST_CASE(test_validate_detects_invalid_inflow_and_credit_modulation, CommonFixture)
{
    east->hydro.reset();
    west->hydro.reset();

    east->hydro.inflowPattern[0][5] = -1.0;
    east->hydro.creditModulation[10][0] = -1.0;

    BOOST_CHECK(!validate());

    // checkInflowPatternAndCredModul only reports invalid values, it does not fix them
    BOOST_CHECK_EQUAL(east->hydro.inflowPattern[0][5], -1.0);
    BOOST_CHECK_EQUAL(east->hydro.creditModulation[10][0], -1.0);
}

BOOST_FIXTURE_TEST_CASE(test_SaveToFolder_then_LoadFromFolder_roundtrip, CommonFixture)
{
    east->hydro.reset();
    west->hydro.reset();
    capacityFolder(); // ensure common/capacity exists before saving
    studyRef().parameters.compatibility.hydroPmax = Parameters::Compatibility::HydroPmax::Hourly;

    BOOST_REQUIRE(saveToFolder(Parameters::Compatibility::HydroPmax::Hourly));

    // Corrupt in-memory state to prove LoadFromFolder actually reloads from disk
    east->hydro.creditModulation.fill(0.);
    east->hydro.inflowPattern.fill(0.);

    BOOST_CHECK(loadFromFolder());

    BOOST_CHECK_CLOSE(east->hydro.creditModulation[0][0], 1.0, 0.0001);
    BOOST_CHECK_CLOSE(east->hydro.inflowPattern[0][0], 1.0, 0.0001);
    BOOST_CHECK_CLOSE(east->hydro.dailyNbHoursAtGenPmax[0][0], 24.0, 0.0001);
}

BOOST_FIXTURE_TEST_CASE(test_LoadFromFolder_missing_files_returns_false, CommonFixture)
{
    // No hydro.ini, no CSV files written: LoadFromFolder must fail gracefully, not crash
    BOOST_CHECK(!loadFromFolder());
}

BOOST_AUTO_TEST_SUITE_END()
