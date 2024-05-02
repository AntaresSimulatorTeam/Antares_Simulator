#define BOOST_TEST_MODULE test hydro series
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include <antares/study/study.h>
#include <antares/array/matrix.h>
#include <files-system.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

void fillTimeSeriesWithSpecialEnds(Matrix<double>& timeSeries, double value)
{
    for (uint ts = 0; ts < timeSeries.width; ts++)
    {
        timeSeries[ts][0] = value + 1;
        timeSeries[ts][timeSeries.height - 1] = value + 2;
    }
}

struct Fixture
{
    Fixture()
    {
        // Create studies
        study = std::make_shared<Study>(true);

        // Add areas to studies
        area_1 = study->areaAdd("Area1");
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

        pathToSeriesFolder.clear();
        pathToSeriesFolder = base_folder + SEP + series_folder;
    }

    void createFoldersAndFiles()
    {
        // series folder
        std::string buffer;
        createFolder(base_folder, series_folder);

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
    }

    std::shared_ptr<Study> study;
    Area* area_1;
    std::string base_folder = fs::temp_directory_path().string();
    std::string series_folder = "series";
    std::string maxHourlyGenPower_file = "maxHourlyGenPower.txt";
    std::string maxHourlyPumpPower_file = "maxHourlyPumpPower.txt";
    std::string pathToMaxHourlyGenPower_file;
    std::string pathToMaxHourlyPumpPower_file;
    std::string pathToSeriesFolder;

    ~Fixture()
    {
        removeFolder(base_folder, series_folder);
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

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

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
    bool usedBySolver = true;

    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower.timeSeries;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower.timeSeries;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

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

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

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

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

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

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

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

    fillTimeSeriesWithSpecialEnds(maxHourlyGenPower, 400.);
    fillTimeSeriesWithSpecialEnds(maxHourlyPumpPower, 200.);

    ret = maxHourlyGenPower.saveToCSVFile(pathToMaxHourlyGenPower_file, 0) && ret;
    ret = maxHourlyPumpPower.saveToCSVFile(pathToMaxHourlyPumpPower_file, 0) && ret;

    maxHourlyGenPower.reset(4, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    ret = area_1->hydro.series->LoadMaxPower(area_1->id, pathToSeriesFolder) && ret;
    BOOST_CHECK(ret);
}

BOOST_AUTO_TEST_SUITE_END()
