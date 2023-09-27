#define BOOST_TEST_MODULE test hydro series

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include "help-functions.h"
#include <study.h>
#include <LoadingError.hpp>

struct Fixture
{
    Fixture()
    {
        // Create studies
        study = make_shared<Study>(true);

        // Add areas to studies
        area_1 = study->areaAdd("Area1");
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        // Instantiating neccessary studies parameters
        study->header.version = 870;
        study->parameters.derated = false;
    }

    void createFoldersAndFiles()
    {
        // series folder
        my_string buffer;
        createFolder(base_folder, series_folder);

        // areas folder
        my_string area1_folder = area_1->id;
        buffer.clear() << base_folder << SEP << series_folder;
        createFolder(buffer, area1_folder);

        // maxHourlyGenPower and maxHourlyPumpPower files
        buffer.clear() << base_folder << SEP << series_folder << SEP << area1_folder;
        createFile(buffer, maxHourlyGenPower_file);
        createFile(buffer, maxHourlyPumpPower_file);
    }

    shared_ptr<Study> study;
    Area* area_1;
    my_string base_folder = fs::current_path().string();
    my_string series_folder = "series";
    my_string maxHourlyGenPower_file = "maxHourlyGenPower.txt";
    my_string maxHourlyPumpPower_file = "maxHourlyPumpPower.txt";

    ~Fixture()
    {
        removeFolder(base_folder, series_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_equal_width, Fixture)
{
    bool ret = false;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer);
    area_1->hydro.series->setCountVariable();
    ret = area_1->hydro.series->postProcessMaxPowerTS(*area_1) && ret;
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, 3);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_matrix_equal_width_and_derated, Fixture)
{
    bool ret = true;
    study->parameters.derated = true;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer) && ret;
    ret = area_1->hydro.series->postProcessMaxPowerTS(*area_1) && ret;
    area_1->hydro.series->setMaxPowerTSWhenDeratedMode(*study);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, 1);
    BOOST_CHECK_EQUAL(maxHourlyPumpPower.width, 1);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_2, Fixture)
{
    bool ret = true;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(2, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(2, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer);
    area_1->hydro.series->setCountVariable();
    BOOST_CHECK(ret);
    BOOST_CHECK_THROW(area_1->hydro.series->postProcessMaxPowerTS(*area_1), Error::ReadingStudy);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_different_width_case_1, Fixture)
{
    bool ret = true;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer);
    area_1->hydro.series->setCountVariable();
    ret = area_1->hydro.series->postProcessMaxPowerTS(*area_1) && ret;
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, maxHourlyPumpPower.width);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_different_width_case_2, Fixture)
{
    bool ret = true;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer);
    area_1->hydro.series->setCountVariable();
    ret = area_1->hydro.series->postProcessMaxPowerTS(*area_1) && ret;
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, maxHourlyPumpPower.width);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_zeros, Fixture)
{
    bool ret = true;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyGenPower_file;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxHourlyPumpPower_file;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(area_1->id, buffer);
    area_1->hydro.series->setCountVariable();
    maxHourlyGenPower.width = 0;
    maxHourlyPumpPower.width = 0;
    ret = area_1->hydro.series->postProcessMaxPowerTS(*area_1) && ret;
    BOOST_CHECK(!ret);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, maxHourlyPumpPower.width);
}


BOOST_AUTO_TEST_SUITE_END()