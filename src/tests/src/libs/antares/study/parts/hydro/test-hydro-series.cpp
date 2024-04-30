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
