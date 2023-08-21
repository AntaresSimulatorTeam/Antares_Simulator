#define BOOST_TEST_MODULE test data transfer

#define WIN32_LEAN_AND_MEAN
#define SEP IO::Separator

#include <boost/test/included/unit_test.hpp>
#include <matrix.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <study.h>

using namespace Antares::Data;

using my_string = Yuni::CString<256, false>;

using namespace std;
using namespace Antares::Data;
namespace fs = std::filesystem;

const my_string maxgentxt = "maxgen.txt";
const my_string maxpumptxt = "maxpump.txt";

void createFolder(const my_string& path, const my_string& folder_name)
{
    fs::path folder_path = fs::path(path.c_str()) / folder_name.c_str();
    fs::create_directory(folder_path);
}

bool createFile(const my_string& folder_path, const my_string& file_name)
{
    // Construct the full path to the file
    fs::path path = fs::path(folder_path.c_str()) / file_name.c_str();

    // Create an output file stream
    std::ofstream outputFile(path);

    if (outputFile.is_open())
    {
        // File was successfully created and is open
        outputFile << "This is a sample content." << std::endl;
        outputFile.close();
        return true;
    }
    else
    {
        // Failed to create or open the file
        return false;
    }
}

void InstantiateMatrix(Matrix<double, Yuni::sint32>& matrix, double seed)
{
    for (uint i = 0; i < matrix.width; i++)
    {
        for (uint hours = 0; hours < HOURS_PER_YEAR; hours++)
        {
            if (hours == 0)
                matrix[i][hours] = seed + 1;

            if (hours == HOURS_PER_YEAR - 1)
                matrix[i][hours] = seed + 2;

            matrix[i][hours] = seed;
        }
    }
}

void InstantiateMatrixDays(Matrix<double>::ColumnType& col, double seed)
{
    for (uint days = 0; days < DAYS_PER_YEAR; days++)
    {
        if (days == 0)
            col[days] = seed + 1;

        if (days == DAYS_PER_YEAR - 1)
            col[days] = seed + 2;

        col[days] = seed;
    }
}

struct Fixture
{
    Fixture()
    {
        study = make_shared<Study>();
        datatransfer = make_shared<DataTransfer>();
        study->inputExtension = "txt";
        // Add areas
        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFolders();

        auto& gen = datatransfer->maxPower[DataTransfer::genMaxP];
        InstantiateMatrixDays(gen, 300.);

        auto& pump = datatransfer->maxPower[DataTransfer::pumpMaxP];
        InstantiateMatrixDays(pump, 200.);

        auto& hoursGen = datatransfer->maxPower[DataTransfer::genMaxE];
        InstantiateMatrixDays(hoursGen, 20.);

        auto& hoursPump = datatransfer->maxPower[DataTransfer::pumpMaxE];
        InstantiateMatrixDays(hoursPump, 14.);
    }

    void createFolders()
    {
        // hydro folder
        my_string buffer;

        createFolder(base_folder, hydro_folder);

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
        my_string file1_name = "maxhoursGen_" + area_2->id + ".txt";
        my_string file2_name = "maxhoursPump_" + area_2->id + ".txt";

        createFile(buffer, file1_name);
        createFile(buffer, file2_name);
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

BOOST_AUTO_TEST_SUITE_END()