#define BOOST_TEST_MODULE test hydro series

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include "help-functions.h"
#include <study.h>

struct Fixture
{
    Fixture()
    {
        // Create studies
        study = make_shared<Study>(true);
        study->inputExtension = "txt";
        studyNoSolver = make_shared<Study>();
        studyNoSolver->inputExtension = "txt";

        // Add areas to studies
        area_1 = study->areaAdd("Area1");
        study->areas.rebuildIndexes();
        area_No_Solver = studyNoSolver->areaAdd("AreaNoSolver");
        studyNoSolver->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        // Instantiating neccessary studies parameters
        study->header.version = 870;
        study->parameters.derated = false;

        studyNoSolver->header.version = 870;
        studyNoSolver->parameters.derated = false;
    }

    void createFoldersAndFiles()
    {
        // series folder
        my_string buffer;
        createFolder(base_folder, series_folder);

        // areas folder
        my_string area1_folder = area_1->id;
        my_string area_No_Solver_folder = area_No_Solver->id;
        buffer.clear() << base_folder << SEP << series_folder;
        createFolder(buffer, area1_folder);
        createFolder(buffer, area_No_Solver_folder);

        // maxHourlyGenPower and maxHourlyPumpPower files
        buffer.clear() << base_folder << SEP << series_folder << SEP << area1_folder;
        createFile(buffer, maxgentxt);
        createFile(buffer, maxpumptxt);

        // maxHourlyGenPower and maxHourlyPumpPower files
        buffer.clear() << base_folder << SEP << series_folder << SEP << area_No_Solver_folder;
        createFile(buffer, maxgentxt);
        createFile(buffer, maxpumptxt);
    }

    shared_ptr<Study> study;
    shared_ptr<Study> studyNoSolver;
    Area* area_1;
    Area* area_No_Solver;
    my_string base_folder = fs::current_path().string();
    my_string series_folder = "series";
    my_string maxgentxt = "maxHourlyGenPower.txt";
    my_string maxpumptxt = "maxHourlyPumpPower.txt";

    ~Fixture()
    {
        removeFolder(base_folder, series_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_matrix_equal_width, Fixture)
{
    bool ret = false;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_1, Fixture)
{
    bool ret = false;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxHourlyGenPower.width, 3);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_2, Fixture)
{
    bool ret = false;
    auto& maxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadMaxPower(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxHourlyPumpPower.width, 3);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_no_solver, Fixture)
{
    bool ret = false;
    auto& maxHourlyGenPower = area_No_Solver->hydro.series->maxHourlyGenPower;
    auto& maxHourlyPumpPower = area_No_Solver->hydro.series->maxHourlyPumpPower;
    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxHourlyGenPower, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxHourlyPumpPower, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_No_Solver->id << SEP
                   << maxgentxt;
    maxHourlyGenPower.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_No_Solver->id << SEP
                   << maxpumptxt;
    maxHourlyPumpPower.saveToCSVFile(buffer, 0);

    maxHourlyGenPower.reset(3, HOURS_PER_YEAR);
    maxHourlyPumpPower.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_No_Solver->hydro.series->LoadMaxPower(
      *studyNoSolver, area_No_Solver->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK(area_No_Solver->hydro.hydroModulable);
}

BOOST_AUTO_TEST_SUITE_END()