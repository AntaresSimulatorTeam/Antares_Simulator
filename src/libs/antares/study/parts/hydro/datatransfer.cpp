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
#include "datatransfer.h"
#include <antares/inifile/inifile.h>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares::Data
{

DataTransfer::DataTransfer()
{
    maxPower.reset(4, DAYS_PER_YEAR, true);
}

bool DataTransfer::LoadFromFolder(Study& study, const AnyString& folder, Area& area)
{
    auto& buffer = study.bufferLoadingTS;
    bool ret = true;

    buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                   << area.id << '.' << study.inputExtension;

    // GUI part patch :
    // We need to know, when estimating the RAM required by the solver, if the current area
    // is hydro modulable. Therefore, reading the area's daily max power at this stage is
    // necessary.

    if (!study.usedByTheSolver)
    {
        bool enabledModeIsChanged = false;
        if (JIT::enabled)
        {
            JIT::enabled = false; // Allowing to read the area's daily max power
            enabledModeIsChanged = true;
        }

        ret = maxPower.loadFromCSVFile(
                buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
              && ret;

        if (enabledModeIsChanged)
            JIT::enabled = true; // Back to the previous loading mode.
    }
    else
    {
        ret = maxPower.loadFromCSVFile(
                buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
              && ret;

        bool errorPowers = false;
        for (uint i = 0; i < 4; ++i)
        {
            auto& col = maxPower[i];
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
    }

    return ret;
}

bool DataTransfer::AutoTransferHours(Study& study, const AnyString& folder, Area& area)
{
    auto& buffer = study.bufferLoadingTS;
    bool ret = true;

    auto& maxHoursGen = area.hydro.maxHoursGen;
    auto& maxHoursPump = area.hydro.maxHoursPump;

    maxHoursGen.reset(1, DAYS_PER_YEAR, true);
    maxHoursGen.fillColumn(0, 24.);

    maxHoursPump.reset(1, DAYS_PER_YEAR, true);
    maxHoursPump.fillColumn(0, 24.);

    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
    {
        maxHoursGen[0][day] = maxPower[genMaxE][day];
        maxHoursPump[0][day] = maxPower[pumpMaxE][day];
    }

    buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxhoursGen_"
                   << area.id << ".txt";
    ret = maxHoursGen.saveToCSVFile(buffer, /*decimal*/ 2) && ret;

    buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxhoursPump_"
                   << area.id << ".txt";
    ret = maxHoursPump.saveToCSVFile(buffer, /*decimal*/ 2) && ret;

    return ret;
}

bool DataTransfer::SupportForOldStudies(Study& study, const AnyString& folder, Area& area)
{
    bool ret = true;
    auto& buffer = study.bufferLoadingTS;
    auto& maxgen = area.hydro.series->maxgen;
    auto& maxpump = area.hydro.series->maxpump;

    auto& MaxPowerGen = maxPower[genMaxP];
    auto& MaxPowerPump = maxPower[pumpMaxP];

    maxgen.reset(1, HOURS_PER_YEAR);
    maxpump.reset(1, HOURS_PER_YEAR);

    AutoTransferPower(maxgen, MaxPowerGen);
    AutoTransferPower(maxpump, MaxPowerPump);

    buffer.clear() << folder << SEP << "series" << SEP << area.id << SEP << "maxgen." << study.inputExtension;
    ret = maxgen.saveToCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    buffer.clear() << folder << SEP << "series" << SEP << area.id << SEP << "maxpump." << study.inputExtension;
    ret = maxpump.saveToCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    return ret;
}

void DataTransfer::AutoTransferPower(Matrix<double, int32_t>& matrix,
                                       const Matrix<double>::ColumnType& maxP)
{
    uint hours = 0;
    uint days = 0;

    while (hours < HOURS_PER_YEAR && days < DAYS_PER_YEAR)
    {
        for (uint i = 0; i < 24; ++i)
        {
            matrix[0][hours] = maxP[days];
            ++hours;
        }
    ++days;
    }
}

} // namespace Antares::Data