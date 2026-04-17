// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test hydro series

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>

#include <boost/test/unit_test.hpp>

#include <antares/array/matrix.h>
#include <antares/study/study.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

void fillTimeSeriesWithSpecialEnds(Matrix<double>& timeSeries, double start, double end)
{
    for (uint ts = 0; ts < timeSeries.width; ts++)
    {
        timeSeries[ts][0] = start;
        timeSeries[ts][timeSeries.height - 1] = end;
    }
}

struct Fixture
{
    Fixture()
    {
        // Create studies
        study = std::make_shared<Study>();

        // Add areas to studies
        area_1 = addAreaToListOfAreas(study->areas, "Area 1");
        if (area_1)
        {
            area_1->createMissingData();
            area_1->resetToDefaultValues();
        }
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        // Instantiating neccessary studies parameters
        study->header.version = Antares::Data::StudyVersion(9, 1);
        study->parameters.derated = false;

        //  Setting necessary paths
        pathToMaxHourlyGenPower_file.clear();
        pathToMaxHourlyGenPower_file = base_folder + SEP + series_folder + SEP + area_1->id.c_str()
                                       + SEP + maxHourlyGenPower_file;

        pathToMaxHourlyPumpPower_file.clear();
        pathToMaxHourlyPumpPower_file = base_folder + SEP + series_folder + SEP + area_1->id.c_str()
                                        + SEP + maxHourlyPumpPower_file;

        pathToMaxDailyReservoirLevels_file.clear();
        pathToMaxDailyReservoirLevels_file = base_folder + SEP + series_folder + SEP
                                             + area_1->id.c_str() + SEP
                                             + maxDailyReservoirLevels_file;

        pathToMinDailyReservoirLevels_file.clear();
        pathToMinDailyReservoirLevels_file = base_folder + SEP + series_folder + SEP
                                             + area_1->id.c_str() + SEP
                                             + minDailyReservoirLevels_file;

        pathToAvgDailyReservoirLevels_file.clear();
        pathToAvgDailyReservoirLevels_file = base_folder + SEP + series_folder + SEP
                                             + area_1->id.c_str() + SEP
                                             + avgDailyReservoirLevels_file;
        pathToReservoirLevels_file.clear();
        pathToReservoirLevels_file = base_folder + SEP + common_folder + SEP + capacity_folder + SEP
                                     + "reservoir_" + area_1->id + ".txt";

        pathToSeriesFolder.clear();
        pathToSeriesFolder = base_folder + SEP + series_folder;

        pathToCommonCapacityFolder.clear();
        pathToCommonCapacityFolder = base_folder + SEP + common_folder + SEP + capacity_folder;
    }

    void createFoldersAndFiles()
    {
        // series folder
        std::string buffer;
        createFolder(base_folder, series_folder);

        // common folder
        createFolder(base_folder, common_folder);

        // capacity folder
        buffer.clear();
        buffer = base_folder + SEP + common_folder;
        createFolder(buffer, capacity_folder);

        // area folder
        std::string area1_folder = area_1->id.c_str();
        buffer.clear();
        buffer = base_folder + SEP + series_folder;
        createFolder(buffer, area1_folder);

        // maxHourlyGenPower and maxHourlyPumpPower files
        buffer.clear();
        buffer = base_folder + SEP + series_folder + SEP + area1_folder;
        createFile(buffer, maxHourlyGenPower_file);
        createFile(buffer, maxHourlyPumpPower_file);
        createFile(buffer, maxDailyReservoirLevels_file);
        createFile(buffer, minDailyReservoirLevels_file);
        createFile(buffer, avgDailyReservoirLevels_file);

        buffer.clear();
        buffer = base_folder + SEP + common_folder + SEP + capacity_folder;
        std::string file_name = "reservoir_" + area_1->id + ".txt";
        createFile(buffer, file_name);
    }

    std::shared_ptr<Study> study;
    Area* area_1;
    std::string base_folder = fs::temp_directory_path().string();
    std::string series_folder = "series";
    std::string common_folder = "common";
    std::string capacity_folder = "capacity";
    std::string maxHourlyGenPower_file = "maxHourlyGenPower.txt";
    std::string maxHourlyPumpPower_file = "maxHourlyPumpPower.txt";
    std::string maxDailyReservoirLevels_file = "maxDailyReservoirLevels.txt";
    std::string minDailyReservoirLevels_file = "minDailyReservoirLevels.txt";
    std::string avgDailyReservoirLevels_file = "avgDailyReservoirLevels.txt";
    std::string pathToMaxHourlyGenPower_file;
    std::string pathToMaxHourlyPumpPower_file;
    std::string pathToMaxDailyReservoirLevels_file;
    std::string pathToMinDailyReservoirLevels_file;
    std::string pathToAvgDailyReservoirLevels_file;
    std::string pathToReservoirLevels_file;
    std::string pathToCommonCapacityFolder;
    std::string pathToSeriesFolder;

    ~Fixture()
    {
        removeFolder(base_folder, series_folder);
        removeFolder(base_folder, common_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_equal_width, Fixture)
{
    bool ret = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_matrix_equal_width_and_derated, Fixture)
{
    bool ret = true;
    study->parameters.derated = true;
    StudyVersion studyVersion(9, 1);

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_2, Fixture)
{
    bool ret = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(2, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(2, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_different_width_case_1, Fixture)
{
    bool ret = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_different_width_case_2, Fixture)
{
    bool ret = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_zeros, Fixture)
{
    bool ret = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 401., 402.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 201., 202.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_matrices_equal_width, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Scenarized;

    auto& maxDailyRuleCurves = area_1->hydro.series->ruleCurves.max.timeSeries;
    auto& minDailyRuleCurves = area_1->hydro.series->ruleCurves.min.timeSeries;
    auto& avgDailyRuleCurves = area_1->hydro.series->ruleCurves.avg.timeSeries;

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxDailyRuleCurves, 0.8, 0.7);
    fillTimeSeriesWithSpecialEnds(minDailyRuleCurves, 0.3, 0.4);
    fillTimeSeriesWithSpecialEnds(avgDailyRuleCurves, 0.5, 0.6);

    ret = maxDailyRuleCurves.saveToCSVFile(pathToMaxDailyReservoirLevels_file, 2) && ret;
    ret = minDailyRuleCurves.saveToCSVFile(pathToMinDailyReservoirLevels_file, 2) && ret;
    ret = avgDailyRuleCurves.saveToCSVFile(pathToAvgDailyReservoirLevels_file, 2) && ret;

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(ret);
    for (size_t i = 0; i < 3; ++i)
    {
        BOOST_CHECK(maxDailyRuleCurves[i][0] == 0.8);
        BOOST_CHECK(maxDailyRuleCurves[i][DAYS_PER_YEAR - 1] == 0.7);

        BOOST_CHECK(minDailyRuleCurves[i][0] == 0.3);
        BOOST_CHECK(minDailyRuleCurves[i][DAYS_PER_YEAR - 1] == 0.4);

        BOOST_CHECK(avgDailyRuleCurves[i][0] == 0.5);
        BOOST_CHECK(avgDailyRuleCurves[i][DAYS_PER_YEAR - 1] == 0.6);
    }
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_from_common_capacity_folder, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Single;

    auto& maxDailyRuleCurves = area_1->hydro.series->ruleCurves.max.timeSeries;
    auto& minDailyRuleCurves = area_1->hydro.series->ruleCurves.min.timeSeries;
    auto& avgDailyRuleCurves = area_1->hydro.series->ruleCurves.avg.timeSeries;
    Matrix<double> ruleCurves;

    ruleCurves.reset(3, DAYS_PER_YEAR, true);

    ruleCurves.fillColumn(RuleCurves::maximum, 1.);
    ruleCurves.fillColumn(RuleCurves::average, 0.5);

    ruleCurves[RuleCurves::maximum][0] = 0.9;
    ruleCurves[RuleCurves::maximum][DAYS_PER_YEAR - 1] = 0.8;

    ruleCurves[RuleCurves::average][0] = 0.5;
    ruleCurves[RuleCurves::average][DAYS_PER_YEAR - 1] = 0.6;

    ruleCurves[RuleCurves::minimum][0] = 0.1;
    ruleCurves[RuleCurves::minimum][DAYS_PER_YEAR - 1] = 0.2;

    ret = ruleCurves.saveToCSVFile(pathToReservoirLevels_file, 2) && ret;

    ruleCurves.reset(3, DAYS_PER_YEAR, true);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(ret);
    BOOST_CHECK(maxDailyRuleCurves[0][0] == 0.9 && maxDailyRuleCurves[0][DAYS_PER_YEAR - 1] == 0.8);
    BOOST_CHECK(avgDailyRuleCurves[0][0] == 0.5 && avgDailyRuleCurves[0][DAYS_PER_YEAR - 1] == 0.6);
    BOOST_CHECK(minDailyRuleCurves[0][0] == 0.1 && minDailyRuleCurves[0][DAYS_PER_YEAR - 1] == 0.2);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_invalid_mode, Fixture)
{
    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    auto invalidMode = static_cast<Parameters::Compatibility::HydroRuleCurves>(-1);

    BOOST_CHECK_THROW(ruleCurvesLoaderService.LoadFromFolder(area_1->id, base_folder, invalidMode),
                      std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_missing_max_file, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Scenarized;

    auto& maxDailyRuleCurves = area_1->hydro.series->ruleCurves.max.timeSeries;
    auto& minDailyRuleCurves = area_1->hydro.series->ruleCurves.min.timeSeries;
    auto& avgDailyRuleCurves = area_1->hydro.series->ruleCurves.avg.timeSeries;

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxDailyRuleCurves, 0.8, 0.7);
    fillTimeSeriesWithSpecialEnds(minDailyRuleCurves, 0.3, 0.4);
    fillTimeSeriesWithSpecialEnds(avgDailyRuleCurves, 0.5, 0.6);

    ret = minDailyRuleCurves.saveToCSVFile(pathToMinDailyReservoirLevels_file, 2) && ret;
    ret = avgDailyRuleCurves.saveToCSVFile(pathToAvgDailyReservoirLevels_file, 2) && ret;

    fs::remove(pathToMaxDailyReservoirLevels_file);

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(!ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_missing_min_file, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Scenarized;

    auto& maxDailyRuleCurves = area_1->hydro.series->ruleCurves.max.timeSeries;
    auto& minDailyRuleCurves = area_1->hydro.series->ruleCurves.min.timeSeries;
    auto& avgDailyRuleCurves = area_1->hydro.series->ruleCurves.avg.timeSeries;

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxDailyRuleCurves, 0.8, 0.7);
    fillTimeSeriesWithSpecialEnds(minDailyRuleCurves, 0.3, 0.4);
    fillTimeSeriesWithSpecialEnds(avgDailyRuleCurves, 0.5, 0.6);

    ret = maxDailyRuleCurves.saveToCSVFile(pathToMaxDailyReservoirLevels_file, 2) && ret;
    ret = avgDailyRuleCurves.saveToCSVFile(pathToAvgDailyReservoirLevels_file, 2) && ret;

    fs::remove(pathToMinDailyReservoirLevels_file);

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(!ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_missing_avg_file, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Scenarized;

    auto& maxDailyRuleCurves = area_1->hydro.series->ruleCurves.max.timeSeries;
    auto& minDailyRuleCurves = area_1->hydro.series->ruleCurves.min.timeSeries;
    auto& avgDailyRuleCurves = area_1->hydro.series->ruleCurves.avg.timeSeries;

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxDailyRuleCurves, 0.8, 0.7);
    fillTimeSeriesWithSpecialEnds(minDailyRuleCurves, 0.3, 0.4);
    fillTimeSeriesWithSpecialEnds(avgDailyRuleCurves, 0.5, 0.6);

    ret = maxDailyRuleCurves.saveToCSVFile(pathToMaxDailyReservoirLevels_file, 2) && ret;
    ret = minDailyRuleCurves.saveToCSVFile(pathToMinDailyReservoirLevels_file, 2) && ret;

    fs::remove(pathToAvgDailyReservoirLevels_file);

    maxDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    minDailyRuleCurves.reset(3, DAYS_PER_YEAR);
    avgDailyRuleCurves.reset(3, DAYS_PER_YEAR);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(!ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_reservoir_levels_missing_reservoir_file, Fixture)
{
    bool ret = true;

    study->parameters.compatibility.hydroRuleCurves = Parameters::Compatibility::HydroRuleCurves::
      Single;

    Matrix<double> standardRuleCurves;
    standardRuleCurves.reset(3L, DAYS_PER_YEAR, true);

    standardRuleCurves.fillColumn(RuleCurves::maximum, 1.);
    standardRuleCurves.fillColumn(RuleCurves::average, 0.5);
    standardRuleCurves[RuleCurves::minimum][0] = 0.1;

    ret = standardRuleCurves.saveToCSVFile(pathToReservoirLevels_file, 2) && ret;

    fs::remove(pathToReservoirLevels_file);

    standardRuleCurves.reset(3, DAYS_PER_YEAR, true);

    RuleCurvesLoaderService ruleCurvesLoaderService(area_1->hydro.series->ruleCurves);

    ret = ruleCurvesLoaderService.LoadFromFolder(area_1->id,
                                                 base_folder,
                                                 study->parameters.compatibility.hydroRuleCurves)
          && ret;

    BOOST_CHECK(!ret);
}

BOOST_AUTO_TEST_SUITE_END()
