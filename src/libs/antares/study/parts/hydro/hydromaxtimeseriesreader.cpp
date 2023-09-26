/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "../../study.h"
#include "hydromaxtimeseriesreader.h"
#include <antares/inifile/inifile.h>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{

HydroMaxTimeSeriesReader::HydroMaxTimeSeriesReader()
{
    dailyMaxPumpAndGen.reset(4, DAYS_PER_YEAR, true);
}

bool HydroMaxTimeSeriesReader::LoadDailyMaxPowersAndEnergies(const AnyString& folder, Area& area)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    bool ret = true;

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                     << area.id << ".txt";

    //  It is necessary to load maxpower_ txt file, whether loading is called from old GUI
    //  or from solver.

    bool enabledModeIsChanged = false;

    if (JIT::enabled)
    {
        JIT::enabled = false; // Allowing to read the area's daily max power and energy
        enabledModeIsChanged = true;
    }

    ret = dailyMaxPumpAndGen.loadFromCSVFile(
            filePath, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &fileContent)
          && ret;

    if (enabledModeIsChanged)
        JIT::enabled = true; // Back to the previous loading mode.

    bool errorPowers = false;
    for (uint i = 0; i < 4; ++i)
    {
        auto& col = dailyMaxPumpAndGen[i];
        for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        {
            if (!errorPowers && (col[day] < 0 || (i % 2 /*column hours*/ && col[day] > 24)))
            {
                logs.error() << area.name << ": invalid power or energy value";
                errorPowers = true;
                ret = false;
            }
        }
    }
    return ret;
}

bool HydroMaxTimeSeriesReader::SaveDailyMaxEnergy(const AnyString& folder, Area& area)
{
    bool ret = true;

    ret = SaveMaxGenerationEnergy(folder, area) && ret;
    ret = SaveMaxPumpingEnergy(folder, area) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::SaveMaxGenerationEnergy(const AnyString& folder, Area& area)
{
    YString filePath;
    auto& maxDailyGenEnergy = area.hydro.maxDailyGenEnergy;
    bool ret = true;

    maxDailyGenEnergy.reset(1, DAYS_PER_YEAR, true);
    maxDailyGenEnergy.fillColumn(0, 24.);

    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        maxDailyGenEnergy[0][day] = dailyMaxPumpAndGen[genMaxE][day];

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                     << "maxDailyGenEnergy_" << area.id << ".txt";

    ret = maxDailyGenEnergy.saveToCSVFile(filePath, 2) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::SaveMaxPumpingEnergy(const AnyString& folder, Area& area)
{
    YString filePath;
    auto& maxDailyPumpEnergy = area.hydro.maxDailyPumpEnergy;
    bool ret = true;

    maxDailyPumpEnergy.reset(1, DAYS_PER_YEAR, true);
    maxDailyPumpEnergy.fillColumn(0, 24.);

    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        maxDailyPumpEnergy[0][day] = dailyMaxPumpAndGen[pumpMaxE][day];

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                     << "maxDailyPumpEnergy_" << area.id << ".txt";
    ret = maxDailyPumpEnergy.saveToCSVFile(filePath, 2) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::SaveDailyMaxPowerAsHourly(const AnyString& folder, Area& area)
{
    bool ret = true;

    ret = SaveDailyMaxGenPowerAsHourly(folder, area) && ret;
    ret = SaveDailyMaxPumpPowerAsHourly(folder, area) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::SaveDailyMaxGenPowerAsHourly(const AnyString& folder, Area& area)
{
    bool ret = true;
    YString filePath;

    auto& maxHourlyGenPower = area.hydro.series->maxHourlyGenPower;
    auto& dailyMaxGen = dailyMaxPumpAndGen[genMaxP];

    maxHourlyGenPower.reset(1, HOURS_PER_YEAR);

    TransferDailyMaxPowerAsHourly(maxHourlyGenPower[0], dailyMaxGen);

    filePath.clear() << folder << SEP << "series" << SEP << area.id << SEP << "maxHourlyGenPower."
                     << "txt";
    ret = maxHourlyGenPower.saveToCSVFile(filePath, 2) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::SaveDailyMaxPumpPowerAsHourly(const AnyString& folder, Area& area)
{
    bool ret = true;
    YString filePath;

    auto& maxHourlyPumpPower = area.hydro.series->maxHourlyPumpPower;
    auto& dailyMaxPump = dailyMaxPumpAndGen[pumpMaxP];

    maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);

    TransferDailyMaxPowerAsHourly(maxHourlyPumpPower[0], dailyMaxPump);

    filePath.clear() << folder << SEP << "series" << SEP << area.id << SEP << "maxHourlyPumpPower."
                     << "txt";
    ret = maxHourlyPumpPower.saveToCSVFile(filePath, 2) && ret;

    return ret;
}

bool HydroMaxTimeSeriesReader::operator()(const AnyString& folder, Area& area)
{
    bool ret = true;

    ret = LoadDailyMaxPowersAndEnergies(folder, area) && ret;
    ret = SaveDailyMaxEnergy(folder, area) && ret;
    ret = SaveDailyMaxPowerAsHourly(folder, area) && ret;

    return ret;
}

void TransferDailyMaxPowerAsHourly(Matrix<double, int32_t>::ColumnType& hourlyColumn,
                                    const Matrix<double>::ColumnType& dailyColumn)
{
    uint hours = 0;
    uint days = 0;

    while (hours < HOURS_PER_YEAR && days < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < 24; ++i)
        {
            hourlyColumn[hours] = dailyColumn[days];
            ++hours;
        }
        ++days;
    }
}

} // namespace Antares::Data