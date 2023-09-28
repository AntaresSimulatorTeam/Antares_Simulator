#define BOOST_TEST_MODULE test data transfer

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include "help-functions.h"
#include <study.h>

struct Fixture
{
    Fixture()
    {
        study = make_shared<Study>(true);
        reader = make_shared<HydroMaxTimeSeriesReader>();
        // Add areas
        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        auto& gen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
        InstantiateColumn(gen, 300., DAYS_PER_YEAR);

        auto& pump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];
        InstantiateColumn(pump, 200., DAYS_PER_YEAR);

        auto& hoursGen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxE];
        InstantiateColumn(hoursGen, 20., DAYS_PER_YEAR);

        auto& hoursPump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxE];
        InstantiateColumn(hoursPump, 14., DAYS_PER_YEAR);

        my_string buffer;
        my_string file_name = "maxpower_" + area_1->id + ".txt";
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder << SEP
                       << capacity_folder << SEP << file_name;
        reader->dailyMaxPumpAndGen.saveToCSVFile(buffer, 2);
    }

    void createFoldersAndFiles()
    {
        my_string buffer;

        // hydro folder
        createFolder(base_folder, hydro_folder);

        // series folder
        buffer.clear() << base_folder << SEP << hydro_folder;
        createFolder(buffer, series_folder);

        // area1 folder
        my_string area1_folder = area_1->id;
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << series_folder;
        createFolder(buffer, area1_folder);
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << series_folder << SEP
                       << area1_folder;
        // maxHourlyGenPower and maxHourlyPumpPower files
        createFile(buffer, maxHourlyGenPower);
        createFile(buffer, maxHourlyPumpPower);

        // common and capacity folders
        buffer.clear() << base_folder << SEP << hydro_folder;
        createFolder(buffer, common_folder);
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder;
        createFolder(buffer, capacity_folder);

        // maxhours files
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder << SEP
                       << capacity_folder;
        my_string file1_name = maxDailyGenEnergy_ << SEP << area_1->id << SEP << ".txt";
        my_string file2_name = maxDailyPumpEnergy_ << SEP << area_1->id << SEP << ".txt";
        my_string file3_name = maxpower << SEP << area_1->id << SEP << ".txt";

        createFile(buffer, file1_name);
        createFile(buffer, file2_name);
        createFile(buffer, file3_name);
    }

    shared_ptr<Study> study;
    shared_ptr<HydroMaxTimeSeriesReader> reader;
    Area* area_1;
    Area* area_2;
    my_string base_folder = fs::current_path().string();
    my_string hydro_folder = "hydro";
    my_string series_folder = "series";
    my_string common_folder = "common";
    my_string capacity_folder = "capacity";
    my_string maxDailyGenEnergy_ = "maxDailyGenEnergy_";
    my_string maxDailyPumpEnergy_ = "maxDailyPumpEnergy_";
    my_string maxpower = "maxpower_";
    my_string maxHourlyGenPower = "maxHourlyGenPower.txt";
    my_string maxHourlyPumpPower = "maxHourlyPumpPower.txt";

    ~Fixture()
    {
        removeFolder(base_folder, hydro_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_support_for_old_studies, Fixture)
{
    my_string buffer;
    bool ret = false;
    auto& colMaxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower[0];
    auto& colMaxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower[0];
    auto& gen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
    auto& pump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];

    buffer.clear() << base_folder << SEP << hydro_folder;
    ret = (*reader)(buffer, *area_1);
    BOOST_CHECK(ret);
    BOOST_CHECK(DailyMaxPowerAsHourlyTransferCheck(colMaxHourlyGenPower, gen));
    BOOST_CHECK(DailyMaxPowerAsHourlyTransferCheck(colMaxHourlyPumpPower, pump));
}

BOOST_AUTO_TEST_SUITE_END()