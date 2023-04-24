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

    buffer.clear() << folder << SEP << areaID << SEP << "ror." << study.inputExtension;

    ret = ror.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    buffer.clear() << folder << SEP << areaID << SEP << "mod." << study.inputExtension;
    ret = storage.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, &study.dataBuffer) && ret;

    // The number of time-series
    count = storage.width;

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

bool DataSeriesHydro::forceReload(bool reload) const
{
    bool ret = true;
    ret = ror.forceReload(reload) && ret;
    ret = storage.forceReload(reload) && ret;
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

} // namespace Data
} // namespace Antares
