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
        datatransfer = make_shared<DataTransfer>();
        study->inputExtension = "txt";
        // Add areas
        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFolders();

        auto& gen = datatransfer->maxPower[DataTransfer::genMaxP];
        InstantiateColumn(gen, 300., DAYS_PER_YEAR);

        auto& pump = datatransfer->maxPower[DataTransfer::pumpMaxP];
        InstantiateColumn(pump, 200., DAYS_PER_YEAR);

        auto& hoursGen = datatransfer->maxPower[DataTransfer::genMaxE];
        InstantiateColumn(hoursGen, 20., DAYS_PER_YEAR);

        auto& hoursPump = datatransfer->maxPower[DataTransfer::pumpMaxE];
        InstantiateColumn(hoursPump, 14., DAYS_PER_YEAR);

        my_string buffer;
        my_string file_name = "maxpower_" + area_2->id + ".txt";
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder << SEP
                       << capacity_folder << SEP << file_name;
        datatransfer->maxPower.saveToCSVFile(buffer, 2);
    }

    void createFolders()
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
        // maxgen and maxpump files
        createFile(buffer, maxgentxt);
        createFile(buffer, maxpumptxt);

        // common and capacity folders
        buffer.clear() << base_folder << SEP << hydro_folder;
        createFolder(buffer, common_folder);
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder;
        createFolder(buffer, capacity_folder);

        // maxhours files
        buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder << SEP
                       << capacity_folder;
        my_string file1_name = maxhoursGen << SEP << area_2->id << SEP << ".txt";
        my_string file2_name = maxhoursPump << SEP << area_2->id << SEP << ".txt";
        my_string file3_name = maxpower << SEP << area_2->id << SEP << ".txt";

        createFile(buffer, file1_name);
        createFile(buffer, file2_name);
        createFile(buffer, file3_name);
    }

    shared_ptr<Study> study;
    shared_ptr<DataTransfer> datatransfer;
    Area* area_1;
    Area* area_2;
    my_string base_folder = fs::current_path().string();
    my_string hydro_folder = "hydro";
    my_string series_folder = "series";
    my_string common_folder = "common";
    my_string capacity_folder = "capacity";
    my_string maxhoursGen = "maxhoursGen_";
    my_string maxhoursPump = "maxhoursPump_";
    my_string maxpower = "maxpower_";
    my_string maxgentxt = "maxgen.txt";
    my_string maxpumptxt = "maxpump.txt";

    ~Fixture()
    {
        fs::path folder_path = fs::path(base_folder.c_str()) / hydro_folder.c_str();
        if (fs::exists(folder_path))
        {
            try
            {
                fs::remove_all(folder_path);
                std::cout << "Folder '" << hydro_folder << "' at '" << folder_path
                          << "' deleted.\n";
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Exception deleting folder '" << hydro_folder << "': " << e.what()
                          << "\n";
            }
        }
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_support_for_old_studies, Fixture)
{
    my_string buffer;
    bool ret = false;

    buffer.clear() << base_folder << SEP << hydro_folder;
    ret = datatransfer->SupportForOldStudies(*study, buffer, *area_1);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_auto_transfer_hours, Fixture)
{
    my_string buffer;
    bool ret = false;

    buffer.clear() << base_folder << SEP << hydro_folder;
    ret = datatransfer->AutoTransferHours(*study, buffer, *area_2);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_from_folder_when_retuns_true, Fixture)
{
    my_string buffer;
    bool ret = false;
    datatransfer->maxPower.reset(4, DAYS_PER_YEAR, true);

    buffer.clear() << base_folder << SEP << hydro_folder;
    ret = datatransfer->LoadFromFolder(*study, buffer, *area_2);
    BOOST_CHECK(ret);
}

BOOST_FIXTURE_TEST_CASE(Testing_load_from_folder_when_retuns_false, Fixture)
{
    my_string buffer;
    bool ret = false;

    auto& hoursGen = datatransfer->maxPower[DataTransfer::genMaxE];
    InstantiateColumn(hoursGen, 25., DAYS_PER_YEAR);

    my_string file_name = "maxpower_" + area_2->id + ".txt";
    buffer.clear() << base_folder << SEP << hydro_folder << SEP << common_folder << SEP
                   << capacity_folder << SEP << file_name;
    datatransfer->maxPower.saveToCSVFile(buffer, 2);
    datatransfer->maxPower.reset(4, DAYS_PER_YEAR, true);

    buffer.clear() << base_folder << SEP << hydro_folder;
    ret = datatransfer->LoadFromFolder(*study, buffer, *area_2);
    BOOST_CHECK(!ret);
}

BOOST_AUTO_TEST_SUITE_END()