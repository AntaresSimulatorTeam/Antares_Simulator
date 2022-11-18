/*
** Copyright 2007-2018 RTE
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

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <stdio.h>
#include "series.h"
#include "../../../inifile.h"
#include "../../../logs.h"
#include "../../study.h"
#include "../../memory-usage.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
DataSeriesHydro::DataSeriesHydro() : count(0)
{
}

bool DataSeriesHydro::saveToFolder(const AreaName& areaID, const AnyString& folder) const
{
    String buffer;
    buffer.clear() << folder << SEP << areaID;
    /* Make sure the folder is created */
    if (IO::Directory::Create(buffer))
    {
        bool ret = true;

        // Saving data
        buffer.clear() << folder << SEP << areaID << SEP << "ror.txt";
        ret = ror.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mod.txt";
        ret = storage.saveToCSVFile(buffer, 0) && ret;

        return ret;
    }
    return false;
}

bool DataSeriesHydro::loadFromFolder(Study& study, const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    auto& buffer = study.bufferLoadingTS;

    // Before 3.9, the file was fatal.txt, and was a matrix N x DAYX_PER_YEAR
    buffer.clear() << folder << SEP << areaID << SEP << "ror." << study.inputExtension;

    if (study.header.version <= 390 && not IO::File::Exists(buffer))
    {
        // We have to perform a conversion
        buffer.clear() << folder << SEP << areaID << SEP << "fatal." << study.inputExtension;
        Matrix<double, sint32> tmp;
        enum
        {
            options = Matrix<>::optImmediate, // | Matrix<>::optQuiet,
        };

        ret = tmp.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, options, &study.dataBuffer) && ret;
        if (ret)
        {
            if (tmp.height != DAYS_PER_YEAR)
            {
                logs.warning() << "hydro: " << areaID << ": ROR: expecting a matrix N x "
                               << DAYS_PER_YEAR;
                ror.reset(1, HOURS_PER_YEAR);
            }
            else
            {
                ror.reset(tmp.width, HOURS_PER_YEAR);
                for (uint x = 0; x != ror.width; ++x)
                {
                    auto& source = tmp[x];
                    auto& target = ror[x];
                    uint hour = 0;
                    for (uint day = 0; day != DAYS_PER_YEAR; ++day)
                    {
                        double v = source[day];
                        for (uint h = 0; h != 24; ++h, ++hour)
                            target[hour] = v;
                    }
                }
            }
        }
        else
        {
            logs.warning() << "hydro: " << areaID << ": reseting to 1x" << HOURS_PER_YEAR
                           << ", failed to load " << buffer;
            ror.reset(1, HOURS_PER_YEAR);
        }
        ror.markAsModified();
    }
    else
    {
        ret = ror.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;
    }
    if (study.header.version >= 620)
    {
        buffer.clear() << folder << SEP << areaID << SEP << "mod." << study.inputExtension;
        ret = storage.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, &study.dataBuffer) && ret;

        // The number of time-series
        count = storage.width;
    }
    else
    {
        bool enabledModeIsChanged = false;
        if (JIT::enabled)
        {
            JIT::enabled = false; // Allowing to read the area's daily max power
            enabledModeIsChanged = true;
        }
        buffer.clear() << folder << SEP << areaID << SEP << "mod." << study.inputExtension;
        ret = storage.loadFromCSVFile(buffer, 1, 12, &study.dataBuffer) && ret;

        // The number of time-series
        count = storage.width;
        const int countInt = storage.width;
        // double temp[countInt][DAYS_PER_YEAR];

        std::vector<std::vector<double>> temp(countInt, std::vector<double>(DAYS_PER_YEAR));

        uint firstDayMonth[13];
        uint daysPerMonthDecals[12];

        AdjustMonth(study, firstDayMonth, daysPerMonthDecals);

        for (int x = 0; x < countInt; x++)
        {
            auto& col = storage[x];
            for (int month = 0; month < 12; month++)
            {
                // Example : month = 0, realMonth = 2 (march), the first month
                // of the simulation (march -> february)
                int realMonth = (month + study.parameters.firstMonthInYear) % 12;

                // Total march power / number of days in march
                int res = (int)(Math::Floor(col[realMonth] / daysPerMonthDecals[month]));
                // Possible difference, always positive and
                // inferior to the number of days in the month
                uint diff = (uint)(col[realMonth] - res * daysPerMonthDecals[month]);

                // First day of march is 0 to < 31
                for (uint day = firstDayMonth[month]; day < firstDayMonth[month + 1]; day++)
                {
                    temp[x][day] = res;
                    if (day - firstDayMonth[month] < diff)
                    {
                        temp[x][day]++;
                    }
                }
            }
        }
        storage.reset(count, DAYS_PER_YEAR, true);
        for (int x = 0; x < countInt; x++)
        {
            auto& col = storage[x];
            for (int i = 0; i < DAYS_PER_YEAR; i++)
            {
                col[i] = temp[x][i];
            }
        }
        if (enabledModeIsChanged)
            JIT::enabled = true; // Back to the previous loading mode.
    }

    if (ror.width > count)
        count = ror.width;

    if (study.usedByTheSolver)
    {
        if (0 == count)
        {
            logs.error() << "Hydro: `" << areaID
                         << "`: empty matrix detected. Fixing it with default values";
            ror.reset(1, DAYS_PER_YEAR);
            storage.reset(1, DAYS_PER_YEAR);
        }
        else
        {
            if (count > 1 && storage.width != ror.width)
            {
                if (ror.width != 1 && storage.width != 1)
                {
                    logs.fatal() << "Hydro: `" << areaID
                                 << "`: The matrices ROR (run-of-the-river) and hydro-storage must "
                                    "have the same number of time-series.";
                    study.gotFatalError = true;
                }
                else
                {
                    if (ror.width == 1)
                    {
                        ror.resizeWithoutDataLost(count, ror.height);
                        for (uint x = 1; x < count; ++x)
                            ror.pasteToColumn(x, ror[0]);
                    }
                    else
                    {
                        if (storage.width == 1)
                        {
                            storage.resizeWithoutDataLost(count, storage.height);
                            for (uint x = 1; x < count; ++x)
                                storage.pasteToColumn(x, storage[0]);
                        }
                    }
                    Area* areaToInvalidate = study.areas.find(areaID);
                    if (areaToInvalidate)
                    {
                        areaToInvalidate->invalidateJIT = true;
                        logs.info()
                          << "  '" << areaID << "': The hydro data have been normalized to "
                          << count << " timeseries";
                    }
                    else
                        logs.error()
                          << "Impossible to find the area `" << areaID << "` to invalidate it";
                }
            }
        }

        if (study.parameters.derated)
        {
            ror.averageTimeseries();
            storage.averageTimeseries();
            count = 1;
        }
    }

    timeseriesNumbers.clear();

    return ret;
}

bool DataSeriesHydro::invalidate(bool reload) const
{
    bool ret = true;
    ret = ror.invalidate(reload) && ret;
    ret = storage.invalidate(reload) && ret;
    return ret;
}

void DataSeriesHydro::markAsModified() const
{
    ror.markAsModified();
    storage.markAsModified();
}

void DataSeriesHydro::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(DataSeriesHydro);
    timeseriesNumbers.estimateMemoryUsage(u, true, 1, u.years);
    // series
    if ((timeSeriesHydro & u.study.parameters.timeSeriesToGenerate))
    {
        ror.estimateMemoryUsage(u, true, u.study.parameters.nbTimeSeriesHydro, HOURS_PER_YEAR);
        storage.estimateMemoryUsage(u, true, u.study.parameters.nbTimeSeriesHydro, 12);
    }
    else
    {
        ror.estimateMemoryUsage(u);
        storage.estimateMemoryUsage(u);
    }
}

void DataSeriesHydro::reset()
{
    ror.reset(1, HOURS_PER_YEAR);
    storage.reset(1, DAYS_PER_YEAR);
    count = 1;
}

uint64 DataSeriesHydro::memoryUsage() const
{
    return sizeof(double) + ror.memoryUsage() + storage.memoryUsage();
}

void DataSeriesHydro::AdjustMonth(const Study& study, uint firstDayMonth[13], uint daysPerMonthDecals[12])
{
    for (int oldMonth = 0; oldMonth < 12; oldMonth++)
    {
        int realMonth = (oldMonth + study.parameters.firstMonthInYear) % 12;
        daysPerMonthDecals[oldMonth] = Constants::daysPerMonth[realMonth];
        if (study.parameters.leapYear)
        {
            if (realMonth == 1) // February
            {
                daysPerMonthDecals[oldMonth]++;
            }
            if (oldMonth == 11) // Last month of the year
            {
                daysPerMonthDecals[oldMonth]--;
            }
        }
    }

    firstDayMonth[0] = 0;
    for (int i = 1; i < 13; i++)
    {
        firstDayMonth[i] = daysPerMonthDecals[i - 1] + firstDayMonth[i - 1];
    }
}

} // namespace Data
} // namespace Antares
