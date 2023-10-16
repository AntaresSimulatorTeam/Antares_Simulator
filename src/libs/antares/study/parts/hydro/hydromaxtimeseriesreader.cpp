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
    dailyMaxPumpAndGen.reset(4U, DAYS_PER_YEAR, true);
}

bool HydroMaxTimeSeriesReader::loadDailyMaxPowersAndEnergies(const AnyString& folder,
                                                             const Area& area,
                                                             bool usedBySolver)
{
    YString filePath;
    Matrix<>::BufferType fileContent;
    bool ret = true;

    filePath.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                     << area.id << ".txt";

    //  It is necessary to load maxpower_ txt file, whether loading is called from old GUI
    //  or from solver.

    if (!usedBySolver)
    {
        bool enabledModeIsChanged = false;

        if (JIT::enabled)
        {
            JIT::enabled = false; // Allowing to read the area's daily max power and energy
            enabledModeIsChanged = true;
        }

        ret = dailyMaxPumpAndGen.loadFromCSVFile(
                filePath, 4U, DAYS_PER_YEAR, Matrix<>::optFixedSize, &fileContent)
              && ret;

        if (enabledModeIsChanged)
            JIT::enabled = true; // Back to the previous loading mode.
    }
    else
    {
        ret = dailyMaxPumpAndGen.loadFromCSVFile(
                filePath, 4U, DAYS_PER_YEAR, Matrix<>::optFixedSize, &fileContent)
              && ret;

        bool errorPowers = false;
        for (uint i = 0; i < 4U; ++i)
        {
            auto& col = dailyMaxPumpAndGen[i];
            for (uint day = 0; day < DAYS_PER_YEAR; ++day)
            {
                if (!errorPowers && (col[day] < 0. || (i % 2U /*column hours*/ && col[day] > 24.)))
                {
                    logs.error() << area.name << ": invalid power or energy value";
                    errorPowers = true;
                    ret = false;
                }
            }
        }
    }
    return ret;
}

void HydroMaxTimeSeriesReader::copyDailyMaxEnergy(Area& area) const
{
    copyDailyMaxGenerationEnergy(area);
    copyDailyMaxPumpingEnergy(area);
}

void HydroMaxTimeSeriesReader::copyDailyMaxGenerationEnergy(Area& area) const
{
    auto& maxDailyGenEnergy = area.hydro.maxDailyGenEnergy;
    const auto& dailyMaxGenE = dailyMaxPumpAndGen[genMaxE];

    maxDailyGenEnergy.reset(1U, DAYS_PER_YEAR, true);

    maxDailyGenEnergy.pasteToColumn(0, dailyMaxGenE);
}

void HydroMaxTimeSeriesReader::copyDailyMaxPumpingEnergy(Area& area) const
{
    auto& maxDailyPumpEnergy = area.hydro.maxDailyPumpEnergy;
    const auto& dailyMaxPumpE = dailyMaxPumpAndGen[pumpMaxE];

    maxDailyPumpEnergy.reset(1U, DAYS_PER_YEAR, true);

    maxDailyPumpEnergy.pasteToColumn(0, dailyMaxPumpE);
}

void HydroMaxTimeSeriesReader::copyDailyMaxPowerAsHourly(Area& area) const
{
    copyDailyMaxGenPowerAsHourly(area);
    copyDailyMaxPumpPowerAsHourly(area);
    area.hydro.series->setNbTimeSeriesSup();
}

void HydroMaxTimeSeriesReader::copyDailyMaxGenPowerAsHourly(Area& area) const
{
    auto& maxHourlyGenPower = area.hydro.series->maxHourlyGenPower;
    const auto& dailyMaxGenP = dailyMaxPumpAndGen[genMaxP];

    maxHourlyGenPower.reset(1U, HOURS_PER_YEAR);

    copyDailyTsAsHourly(maxHourlyGenPower[0], dailyMaxGenP);
}

void HydroMaxTimeSeriesReader::copyDailyMaxPumpPowerAsHourly(Area& area) const
{
    auto& maxHourlyPumpPower = area.hydro.series->maxHourlyPumpPower;
    const auto& dailyMaxPumpP = dailyMaxPumpAndGen[pumpMaxP];

    maxHourlyPumpPower.reset(1U, HOURS_PER_YEAR);

    copyDailyTsAsHourly(maxHourlyPumpPower[0], dailyMaxPumpP);
}

bool HydroMaxTimeSeriesReader::operator()(const AnyString& folder, Area& area, bool usedBySolver)
{
    bool ret = true;

    ret = loadDailyMaxPowersAndEnergies(folder, area, usedBySolver) && ret;
    copyDailyMaxEnergy(area);
    copyDailyMaxPowerAsHourly(area);

    return ret;
}

void copyDailyTsAsHourly(Matrix<double, int32_t>::ColumnType& hourlyColumn,
                         const Matrix<double>::ColumnType& dailyColumn)
{
    uint hour = 0;
    uint day = 0;

    while (hour < HOURS_PER_YEAR && day < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < HOURS_PER_DAY; ++i)
        {
            hourlyColumn[hour] = dailyColumn[day];
            ++hour;
        }
        ++day;
    }
}

} // namespace Antares::Data