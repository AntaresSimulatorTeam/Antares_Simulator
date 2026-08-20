// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro common

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>
#include <memory>

#include <boost/test/unit_test.hpp>

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
        tmp(fs::temp_directory_path()),
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
        return east->hydro.LoadIniFile(*study, tmp) && west->hydro.LoadIniFile(*study, tmp);
    }

    bool loadOnce()
    {
        return PartHydro::LoadIniFile(*study, tmp);
    }

    ~CommonFixture()
    {
        fs::remove(hydroIni);
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
    BOOST_CHECK(!loadOnce());
}

BOOST_FIXTURE_TEST_CASE(test_empty_file_returns_true, CommonFixture)
{
    writeFile("");
    BOOST_CHECK(loadOnce());
}

BOOST_FIXTURE_TEST_CASE(test_inter_daily_breakdown, CommonFixture)
{
    writeFile("[inter-daily-breakdown]\neast = 0.5\nwest = 0.75\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
    BOOST_CHECK_EQUAL(west->hydro.interDailyBreakdown, 0.75);
}

BOOST_FIXTURE_TEST_CASE(test_intra_daily_modulation, CommonFixture)
{
    writeFile("[intra-daily-modulation]\neast = 12.0\nwest = 6.0\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.intraDailyModulation, 12.0);
    BOOST_CHECK_EQUAL(west->hydro.intraDailyModulation, 6.0);
}

BOOST_FIXTURE_TEST_CASE(test_inter_monthly_breakdown, CommonFixture)
{
    writeFile("[inter-monthly-breakdown]\neast = 0.3\nwest = 0.9\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.intermonthlyBreakdown, 0.3);
    BOOST_CHECK_EQUAL(west->hydro.intermonthlyBreakdown, 0.9);
}

BOOST_FIXTURE_TEST_CASE(test_reservoir_management, CommonFixture)
{
    writeFile("[reservoir]\neast = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.reservoirManagement, true);
    BOOST_CHECK_EQUAL(west->hydro.reservoirManagement, false);
}

BOOST_FIXTURE_TEST_CASE(test_reservoir_capacity, CommonFixture)
{
    writeFile("[reservoir capacity]\neast = 1500.0\nwest = 750.5\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.reservoirCapacity, 1500.0);
    BOOST_CHECK_EQUAL(west->hydro.reservoirCapacity, 750.5);
}

BOOST_FIXTURE_TEST_CASE(test_follow_load_modulations, CommonFixture)
{
    writeFile("[follow load]\neast = false\nwest = false\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.followLoadModulations, false);
    BOOST_CHECK_EQUAL(west->hydro.followLoadModulations, false);
}

BOOST_FIXTURE_TEST_CASE(test_use_water_value, CommonFixture)
{
    writeFile("[use water]\neast = true\nwest = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.useWaterValue, true);
    BOOST_CHECK_EQUAL(west->hydro.useWaterValue, true);
}

BOOST_FIXTURE_TEST_CASE(test_hard_bounds_on_rule_curves, CommonFixture)
{
    writeFile("[hard bounds]\neast = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.hardBoundsOnRuleCurves, true);
    BOOST_CHECK_EQUAL(west->hydro.hardBoundsOnRuleCurves, false);
}

BOOST_FIXTURE_TEST_CASE(test_use_heuristic_target, CommonFixture)
{
    writeFile("[use heuristic]\neast = false\nwest = false\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.useHeuristicTarget, false);
    BOOST_CHECK_EQUAL(west->hydro.useHeuristicTarget, false);
}

BOOST_FIXTURE_TEST_CASE(test_power_to_level, CommonFixture)
{
    writeFile("[power to level]\neast = true\nwest = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.powerToLevel, true);
    BOOST_CHECK_EQUAL(west->hydro.powerToLevel, true);
}

BOOST_FIXTURE_TEST_CASE(test_initialize_reservoir_level_date, CommonFixture)
{
    writeFile("[initialize reservoir date]\neast = 5\nwest = 11\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.initializeReservoirLevelDate, 5);
    BOOST_CHECK_EQUAL(west->hydro.initializeReservoirLevelDate, 11);
}

BOOST_FIXTURE_TEST_CASE(test_use_leeway, CommonFixture)
{
    writeFile("[use leeway]\neast = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.useLeeway, true);
    BOOST_CHECK_EQUAL(west->hydro.useLeeway, false);
}

BOOST_FIXTURE_TEST_CASE(test_leeway_lower_bound, CommonFixture)
{
    writeFile("[leeway low]\neast = 0.2\nwest = 0.4\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 0.2);
    BOOST_CHECK_EQUAL(west->hydro.leewayLowerBound, 0.4);
}

BOOST_FIXTURE_TEST_CASE(test_leeway_upper_bound, CommonFixture)
{
    writeFile("[leeway up]\neast = 0.8\nwest = 0.6\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 0.8);
    BOOST_CHECK_EQUAL(west->hydro.leewayUpperBound, 0.6);
}

BOOST_FIXTURE_TEST_CASE(test_pumping_efficiency, CommonFixture)
{
    writeFile("[pumping efficiency]\neast = 0.85\nwest = 0.9\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.pumpingEfficiency, 0.85);
    BOOST_CHECK_EQUAL(west->hydro.pumpingEfficiency, 0.9);
}

BOOST_FIXTURE_TEST_CASE(test_unknown_area_returns_false, CommonFixture)
{
    writeFile("[inter-daily-breakdown]\neast = 0.5\nunknown = 0.3\n");
    BOOST_CHECK(!loadOnce());
    // Known area was written before the unknown one, so it is still loaded
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
}

BOOST_FIXTURE_TEST_CASE(test_empty_section_returns_false, CommonFixture)
{
    // Section header present but no key=value pairs — firstProperty is null
    writeFile("[inter-daily-breakdown]\n");
    BOOST_CHECK(!loadOnce());
}

BOOST_FIXTURE_TEST_CASE(test_case_insensitive_area_name, CommonFixture)
{
    // Area IDs are stored as lowercase; keys from the INI file are also lowercased before lookup
    writeFile("[inter-daily-breakdown]\nEast = 0.5\nWEST = 0.75\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.interDailyBreakdown, 0.5);
    BOOST_CHECK_EQUAL(west->hydro.interDailyBreakdown, 0.75);
}

BOOST_FIXTURE_TEST_CASE(test_absent_section_preserves_constructor_default, CommonFixture)
{
    // Only write one section; other properties must keep their PartHydro() defaults
    writeFile("[reservoir]\neast = true\n");
    BOOST_CHECK(loadOnce());
    BOOST_CHECK_EQUAL(east->hydro.reservoirManagement, true);
    // Constructor defaults for untouched fields
    BOOST_CHECK_EQUAL(east->hydro.overflowSpilledCostDifference, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.followLoadModulations, true);
    BOOST_CHECK_EQUAL(east->hydro.useWaterValue, false);
    BOOST_CHECK_EQUAL(east->hydro.pumpingEfficiency, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.leewayLowerBound, 1.0);
    BOOST_CHECK_EQUAL(east->hydro.leewayUpperBound, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
