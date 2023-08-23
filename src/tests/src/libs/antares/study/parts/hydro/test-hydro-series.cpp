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

        // maxgen and maxpump files
        buffer.clear() << base_folder << SEP << series_folder << SEP << area1_folder;
        createFile(buffer, maxgentxt);
        createFile(buffer, maxpumptxt);

        // maxgen and maxpump files
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
    my_string maxgentxt = "maxgen.txt";
    my_string maxpumptxt = "maxpump.txt";

    ~Fixture()
    {
        fs::path folder_path = fs::path(base_folder.c_str()) / series_folder.c_str();
        if (fs::exists(folder_path))
        {
            try
            {
                fs::remove_all(folder_path);
                std::cout << "Folder '" << series_folder << "' at '" << folder_path
                          << "' deleted.\n";
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Exception deleting folder '" << series_folder << "': " << e.what()
                          << "\n";
            }
        }
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_both_matrix_equal_width, Fixture)
{
    bool ret = false;
    auto& maxgen = area_1->hydro.series->maxgen;
    auto& maxpump = area_1->hydro.series->maxpump;
    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxgen, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxpump, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxgen.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxpump.saveToCSVFile(buffer, 0);

    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadHydroPowerCredits(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_1, Fixture)
{
    bool ret = false;
    auto& maxgen = area_1->hydro.series->maxgen;
    auto& maxpump = area_1->hydro.series->maxpump;
    maxgen.reset(1, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxgen, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxpump, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxgen.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxpump.saveToCSVFile(buffer, 0);

    maxgen.reset(1, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadHydroPowerCredits(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxgen.width, 3);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_matrices_different_width_case_2, Fixture)
{
    bool ret = false;
    auto& maxgen = area_1->hydro.series->maxgen;
    auto& maxpump = area_1->hydro.series->maxpump;
    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(1, HOURS_PER_YEAR);

    InstantiateMatrix(maxgen, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxpump, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxgentxt;
    maxgen.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_1->id << SEP << maxpumptxt;
    maxpump.saveToCSVFile(buffer, 0);

    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(1, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_1->hydro.series->LoadHydroPowerCredits(*study, area_1->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK_EQUAL(maxpump.width, 3);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_power_credits_no_solver, Fixture)
{
    bool ret = false;
    auto& maxgen = area_No_Solver->hydro.series->maxgen;
    auto& maxpump = area_No_Solver->hydro.series->maxpump;
    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    InstantiateMatrix(maxgen, 400., HOURS_PER_YEAR);
    InstantiateMatrix(maxpump, 200., HOURS_PER_YEAR);

    my_string buffer;
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_No_Solver->id << SEP
                   << maxgentxt;
    maxgen.saveToCSVFile(buffer, 0);
    buffer.clear() << base_folder << SEP << series_folder << SEP << area_No_Solver->id << SEP
                   << maxpumptxt;
    maxpump.saveToCSVFile(buffer, 0);

    maxgen.reset(3, HOURS_PER_YEAR);
    maxpump.reset(3, HOURS_PER_YEAR);

    buffer.clear() << base_folder << SEP << series_folder;
    ret = area_No_Solver->hydro.series->LoadHydroPowerCredits(
      *studyNoSolver, area_No_Solver->id, buffer);
    BOOST_CHECK(ret);
    BOOST_CHECK(area_No_Solver->hydro.hydroModulable);
}

BOOST_AUTO_TEST_SUITE_END()