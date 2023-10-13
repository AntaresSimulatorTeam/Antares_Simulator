#define BOOST_TEST_MODULE test data transfer

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
#include <study.h>
#include <matrix.h>
#include <utils.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

bool DailyMaxPowerAsHourlyTransferCheck(Matrix<double, int32_t>::ColumnType& hourlyColumn,
                                        const Matrix<double>::ColumnType& dailyColumn)
{
    uint hours = 0;
    uint days = 0;
    bool check = true;

    while (hours < HOURS_PER_YEAR && days < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < 24; ++i)
        {
            if (hourlyColumn[hours] != dailyColumn[days])
            {
                check = false;
                break;
            }
            ++hours;
        }

        if (!check)
            break;

        ++days;
    }
    return check;
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
        Yuni::CString<256, false> temp(buffer);
        reader->dailyMaxPumpAndGen.saveToCSVFile(temp, 2);
    }

    void createFoldersAndFiles()
    {
        stringT buffer;
        buffer.clear();

        // hydro folder
        createFolder(base_folder, hydro_folder);

        // series folder
        buffer = base_folder + SEP + hydro_folder;
        createFolder(buffer, series_folder);

        // area1 folder
        stringT area1_folder = area_1->id.c_str();
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + series_folder;
        createFolder(buffer, area1_folder);
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + series_folder + SEP + area1_folder;
        // maxHourlyGenPower and maxHourlyPumpPower files
        createFile(buffer, maxHourlyGenPower);
        createFile(buffer, maxHourlyPumpPower);

        // common and capacity folders
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder;
        createFolder(buffer, common_folder);
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder;
        createFolder(buffer, capacity_folder);

        // maxhours files
        stringT maxpowerArea1 = maxpower + area1_folder + ".txt";
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder + SEP + capacity_folder;

        createFile(buffer, maxDailyGenEnergy_);
        createFile(buffer, maxDailyPumpEnergy_);
        createFile(buffer, maxpowerArea1);
    }

    std::shared_ptr<Study> study;
    std::shared_ptr<HydroMaxTimeSeriesReader> reader;
    Area* area_1;
    Area* area_2;
    stringT base_folder = fs::current_path().string();
    stringT hydro_folder = "hydro";
    stringT series_folder = "series";
    stringT common_folder = "common";
    stringT capacity_folder = "capacity";
    stringT maxDailyGenEnergy_ = "maxDailyGenEnergy_area1.txt";
    stringT maxDailyPumpEnergy_ = "maxDailyPumpEnergy_area1.txt";
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
    auto& colMaxHourlyGenPower = area_1->hydro.series->maxHourlyGenPower[0];
    auto& colMaxHourlyPumpPower = area_1->hydro.series->maxHourlyPumpPower[0];
    auto& gen = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
    auto& pump = reader->dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];

    buffer.clear();
    buffer = base_folder + SEP + hydro_folder;
    ret = (*reader)(buffer, *area_1) && ret;
    BOOST_CHECK(ret);
    BOOST_CHECK(DailyMaxPowerAsHourlyTransferCheck(colMaxHourlyGenPower, gen));
    BOOST_CHECK(DailyMaxPowerAsHourlyTransferCheck(colMaxHourlyPumpPower, pump));
}

BOOST_AUTO_TEST_SUITE_END()