/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define BOOST_TEST_MODULE test hydro reader

#define WIN32_LEAN_AND_MEAN

#include <files-system.h>

#include <boost/test/unit_test.hpp>

#include <antares/array/matrix.h>
#include <antares/study/study.h>

#define SEP "/"

using namespace Antares::Data;
namespace fs = std::filesystem;

bool equalDailyMaxPowerAsHourlyTs(const Matrix<double>::ColumnType& hourlyColumn,
                                  const Matrix<double>::ColumnType& dailyColumn)
{
    uint hour = 0;
    uint day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < HOURS_PER_DAY; ++i)
        {
            if (hourlyColumn[hour] != dailyColumn[day])
            {
                return false;
            }
            ++hour;
        }
        ++day;
    }

    return true;
}

bool equalDailyMaxEnergyTs(const Matrix<double>::ColumnType& col1,
                           const Matrix<double>::ColumnType& col2)
{
    for (uint h = 0; h < DAYS_PER_YEAR; ++h)
    {
        if (col1[h] != col2[h])
        {
            return false;
        }
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

        // Add areas
        area_1 = study->areaAdd("Area1");
        study->areas.rebuildIndexes();
        dailyMaxPumpAndGen.reset(4U, DAYS_PER_YEAR);
        reader = std::make_shared<HydroMaxTimeSeriesReader>(area_1->hydro,
                                                            area_1->id.to<std::string>(),
                                                            area_1->name.to<std::string>());

        // Create necessary folders and files for these two areas
        createFoldersAndFiles();

        auto& gen = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
        fillColumnWithSpecialEnds(gen, 300., DAYS_PER_YEAR);

        auto& pump = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];
        fillColumnWithSpecialEnds(pump, 200., DAYS_PER_YEAR);

        auto& hoursGen = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxE];
        fillColumnWithSpecialEnds(hoursGen, 20., DAYS_PER_YEAR);

        auto& hoursPump = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxE];
        fillColumnWithSpecialEnds(hoursPump, 14., DAYS_PER_YEAR);

        std::string buffer;
        buffer.clear();
        buffer = base_folder + SEP + hydro_folder + SEP + common_folder + SEP + capacity_folder
                 + SEP + maxpower + area_1->id.c_str() + ".txt";
        dailyMaxPumpAndGen.saveToCSVFile(buffer, 2);
    }

    void createFoldersAndFiles()
    {
        std::string buffer;
        std::string area1_ID = area_1->id.c_str();
        std::string maxpowerArea1 = maxpower + area1_ID + ".txt";
        std::string maxDailyGenEnergy_Area1 = maxDailyGenEnergy_ + area1_ID + ".txt";
        std::string maxDailyPumpEnergy_Area1 = maxDailyPumpEnergy_ + area1_ID + ".txt";

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
    std::string base_folder = fs::temp_directory_path().string();
    std::string hydro_folder = "hydro";
    std::string series_folder = "series";
    std::string common_folder = "common";
    std::string capacity_folder = "capacity";
    std::string maxDailyGenEnergy_ = "maxDailyGenEnergy_";
    std::string maxDailyPumpEnergy_ = "maxDailyPumpEnergy_";
    std::string maxpower = "maxpower_";
    std::string maxHourlyGenPower = "maxHourlyGenPower.txt";
    std::string maxHourlyPumpPower = "maxHourlyPumpPower.txt";
    Matrix<double> dailyMaxPumpAndGen;

    ~Fixture()
    {
        removeFolder(base_folder, hydro_folder);
    }
};

BOOST_AUTO_TEST_SUITE(s)

BOOST_FIXTURE_TEST_CASE(Testing_support_for_old_studies, Fixture)
{
    std::string buffer;
    bool ret = true;

    auto& genP = area_1->hydro.series->maxHourlyGenPower.timeSeries[0];
    auto& pumpP = area_1->hydro.series->maxHourlyPumpPower.timeSeries[0];
    auto& genE = area_1->hydro.dailyNbHoursAtGenPmax[0];
    auto& pumpE = area_1->hydro.dailyNbHoursAtPumpPmax[0];

    auto& genPReader = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxP];
    auto& pumpPReader = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxP];
    auto& genEReader = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::genMaxE];
    auto& pumpEReader = dailyMaxPumpAndGen[HydroMaxTimeSeriesReader::pumpMaxE];

    buffer.clear();
    buffer = base_folder + SEP + hydro_folder;
    ret = reader->read(buffer, study->usedByTheSolver) && ret;

    BOOST_CHECK(ret);
    BOOST_CHECK(equalDailyMaxPowerAsHourlyTs(genP, genPReader));
    BOOST_CHECK(equalDailyMaxPowerAsHourlyTs(pumpP, pumpPReader));
    BOOST_CHECK(equalDailyMaxEnergyTs(genE, genEReader));
    BOOST_CHECK(equalDailyMaxEnergyTs(pumpE, pumpEReader));
}

BOOST_AUTO_TEST_SUITE_END()
