#define BOOST_TEST_MODULE test data transfer

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <study.h>
#include <matrix.h>
#include <files-system.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

bool equalDailyMaxPowerAsHourlyTs(const Matrix<double, int32_t>::ColumnType& hourlyColumn,
                                        const Matrix<double>::ColumnType& dailyColumn)
{
    uint hour = 0;
    uint day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < HOURS_PER_DAY; ++i)
        {
            if (hourlyColumn[hour] != dailyColumn[day])
                return false;
            ++hour;
        }
        ++day;
    }

    return true;
}

bool equalDailyMaxEnergyTs(const Matrix<double>::ColumnType& col1, const Matrix<double>::ColumnType& col2)
{
    for (uint h = 0; h < DAYS_PER_YEAR; ++h)
    {
        if (col1[h] != col2[h])
            return false;
    }

    return true;
}

void fillColumnWithSpecialEnds(Matrix<double>::ColumnType& col, double value, uint heigth)
{
    col[0] = value + 1;
    col[heigth - 1] = value + 2;
}

struct Fixture
{
    Fixture()
    {
        study = std::make_shared<Study>(true);
        reader = std::make_shared<HydroMaxTimeSeriesReader>();
        // Add areas
        area_1 = study->areaAdd("Area1");
        study->areas.rebuildIndexes();

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        auto& gen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
        fillColumnWithSpecialEnds(gen, 300., DAYS_PER_YEAR);

        auto& pump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];
        fillColumnWithSpecialEnds(pump, 200., DAYS_PER_YEAR);

        auto& hoursGen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxE];
        fillColumnWithSpecialEnds(hoursGen, 20., DAYS_PER_YEAR);

        auto& hoursPump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxE];
        fillColumnWithSpecialEnds(hoursPump, 14., DAYS_PER_YEAR);

        stringT buffer;
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder + SEP + capacity_folder
                 + SEP + maxpower + area_1->id.c_str() + ".txt";
        reader->dailyMaxPumpAndGen.saveToCSVFile(buffer, 2);

        // Reset columns
        reader->dailyMaxPumpAndGen.fillColumn(HydroMaxTimeSeriesReader::genMaxP, 0.);
        reader->dailyMaxPumpAndGen.fillColumn(HydroMaxTimeSeriesReader::pumpMaxP, 0.);
        reader->dailyMaxPumpAndGen.fillColumn(HydroMaxTimeSeriesReader::genMaxE, 24.);
        reader->dailyMaxPumpAndGen.fillColumn(HydroMaxTimeSeriesReader::pumpMaxE, 24.);
    }

    void createFoldersAndFiles()
    {
        stringT buffer;
        stringT area1_ID = area_1->id.c_str();
        stringT maxpowerArea1 = maxpower + area1_ID + ".txt";
        stringT maxDailyGenEnergy_Area1 = maxDailyGenEnergy_ + area1_ID + ".txt";
        stringT maxDailyPumpEnergy_Area1 = maxDailyPumpEnergy_ + area1_ID + ".txt";

        buffer.clear();

        // hydro folder
        createFolder(base_folder, hydro_folder);

        // series folder
        buffer = base_folder + SEP + hydro_folder;
        createFolder(buffer, series_folder);

        // area1 folder
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + series_folder;
        createFolder(buffer, area1_ID);
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + series_folder + SEP + area1_ID;

        //  maxHourlyGenPower and maxHourlyPumpPower files
        createFile(buffer, maxHourlyGenPower);
        createFile(buffer, maxHourlyPumpPower);

        //  common and capacity folders
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder;
        createFolder(buffer, common_folder);
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder;
        createFolder(buffer, capacity_folder);

        //  max daily energy and power file
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder + SEP + capacity_folder;
        createFile(buffer, maxpowerArea1);

        //  max daily energy files
        createFile(buffer, maxDailyGenEnergy_Area1);
        createFile(buffer, maxDailyPumpEnergy_Area1);
    }

    std::shared_ptr<Study> study;
    std::shared_ptr<HydroMaxTimeSeriesReader> reader;
    Area* area_1;
    stringT base_folder = fs::temp_directory_path().string();
    stringT hydro_folder = "hydro";
    stringT series_folder = "series";
    stringT common_folder = "common";
    stringT capacity_folder = "capacity";
    stringT maxDailyGenEnergy_ = "maxDailyGenEnergy_";
    stringT maxDailyPumpEnergy_ = "maxDailyPumpEnergy_";
    stringT maxpower = "maxpower_";
    stringT maxHourlyGenPower = "maxHourlyGenPower.txt";
    stringT maxHourlyPumpPower = "maxHourlyPumpPower.txt";

    ~Fixture()
    {
        removeFolder(base_folder, hydro_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_support_for_old_studies, Fixture)
{
    stringT buffer;
    bool ret = true;

    auto& genP = area_1->hydro.series->maxHourlyGenPower.timeSeries[0];
    auto& pumpP = area_1->hydro.series->maxHourlyPumpPower.timeSeries[0];
    auto& genE = area_1->hydro.maxDailyGenEnergy[0];
    auto& pumpE = area_1->hydro.maxDailyPumpEnergy[0];

    auto& genPReader = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
    auto& pumpPReader = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];
    auto& genEReader = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxE];
    auto& pumpEReader = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxE];

    buffer.clear();
    buffer = base_folder + SEP + hydro_folder;
    ret = (*reader)(buffer, *area_1, study->usedByTheSolver) && ret;

    BOOST_CHECK(ret);
    BOOST_CHECK(equalDailyMaxPowerAsHourlyTs(genP, genPReader));
    BOOST_CHECK(equalDailyMaxPowerAsHourlyTs(pumpP, pumpPReader));
    BOOST_CHECK(equalDailyMaxEnergyTs(genE, genEReader));
    BOOST_CHECK(equalDailyMaxEnergyTs(pumpE, pumpEReader));
}

BOOST_AUTO_TEST_SUITE_END()