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
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include "../../study.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
DataSeriesHydro::DataSeriesHydro() :
    ror(timeseriesNumbers),
    storage(timeseriesNumbers),
    mingen(timeseriesNumbers),
    count(0)
{
    // Pmin was introduced in v8.6
    // The previous behavior was Pmin=0
    // For compatibility reasons with existing studies, mingen is set to one column of zeros
    // by default
    mingen.reset();
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
        ret = ror.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mod.txt";
        ret = storage.timeSeries.saveToCSVFile(buffer, 0) && ret;
        buffer.clear() << folder << SEP << areaID << SEP << "mingen.txt";
        ret = mingen.timeSeries.saveToCSVFile(buffer, 0) && ret;
        return ret;
    }
    return false;
}

bool DataSeriesHydro::loadFromFolder(Study& study, const AreaName& areaID, const AnyString& folder)
{
    bool ret = true;
    auto& buffer = study.bufferLoadingTS;

    buffer.clear() << folder << SEP << areaID << SEP << "ror." << study.inputExtension;

    ret = ror.timeSeries.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;

    buffer.clear() << folder << SEP << areaID << SEP << "mod." << study.inputExtension;
    ret = storage.timeSeries.loadFromCSVFile(buffer, 1, DAYS_PER_YEAR, &study.dataBuffer) && ret;

    // The number of time-series
    count = storage.timeSeries.width;

    if (ror.timeSeries.width > count)
        count = ror.timeSeries.width;

    if (study.header.version >= 860)
    {
        buffer.clear() << folder << SEP << areaID << SEP << "mingen." << study.inputExtension;
        ret = mingen.timeSeries.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &study.dataBuffer) && ret;
    }

    if (study.usedByTheSolver)
    {
        if (0 == count)
        {
            logs.error() << "Hydro: `" << areaID
                         << "`: empty matrix detected. Fixing it with default values";
            ror.reset();
            storage.reset();
            mingen.reset();
        }
        else
        {
            if (count > 1 && storage.timeSeries.width != ror.timeSeries.width)
            {
                if (ror.timeSeries.width != 1 && storage.timeSeries.width != 1)
                {
                    logs.fatal() << "Hydro: `" << areaID
                                 << "`: The matrices ROR (run-of-the-river) and hydro-storage must "
                                    "have the same number of time-series.";
                    study.gotFatalError = true;
                }
                else
                {
                    if (ror.timeSeries.width == 1)
                    {
                        ror.timeSeries.resizeWithoutDataLost(count, ror.timeSeries.height);
                        for (uint x = 1; x < count; ++x)
                            ror.timeSeries.pasteToColumn(x, ror[0]);
                    }
                    else
                    {
                        if (storage.timeSeries.width == 1)
                        {
                            storage.timeSeries.resizeWithoutDataLost(count, storage.timeSeries.height);
                            for (uint x = 1; x < count; ++x)
                                storage.timeSeries.pasteToColumn(x, storage[0]);
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
            checkMinGenTsNumber(study, areaID);
        }

        if (study.parameters.derated)
        {
            ror.averageTimeseries();
            storage.averageTimeseries();
            mingen.averageTimeseries();
            count = 1;
        }
    }

    timeseriesNumbers.clear();

    return ret;
}

void DataSeriesHydro::checkMinGenTsNumber(Study& study, const AreaName& areaID)
{
    if (mingen.timeSeries.width != storage.timeSeries.width)
    {
        if (mingen.timeSeries.width > 1)
        {
            logs.fatal() << "Hydro: `" << areaID
                         << "`: The matrices Minimum Generation must "
                            "has the same number of time-series as ROR and hydro-storage.";
            study.gotFatalError = true;
        }
        else
        {
            mingen.timeSeries.resizeWithoutDataLost(count, mingen.timeSeries.height);
            for (uint x = 1; x < count; ++x)
                mingen.timeSeries.pasteToColumn(x, mingen[0]);
            Area* areaToInvalidate = study.areas.find(areaID);
            if (areaToInvalidate)
            {
                areaToInvalidate->invalidateJIT = true;
                logs.info() << "  '" << areaID
                            << "': The hydro minimum generation data have been normalized to "
                            << count << " timeseries";
            }
            else
                logs.error() << "Impossible to find the area `" << areaID << "` to invalidate it";
        }
    }
}

bool DataSeriesHydro::forceReload(bool reload) const
{
    bool ret = true;
    ret = ror.forceReload(reload) && ret;
    ret = storage.forceReload(reload) && ret;
    ret = mingen.forceReload(reload) && ret;
    return ret;
}

void DataSeriesHydro::markAsModified() const
{
    ror.markAsModified();
    storage.markAsModified();
    mingen.markAsModified();
}

void DataSeriesHydro::reset()
{
    ror.reset();
    storage.reset();
    mingen.reset();
    count = 1;
}

uint64_t DataSeriesHydro::memoryUsage() const
{
    return sizeof(double) + ror.memoryUsage() + storage.memoryUsage() + mingen.memoryUsage();
}

unsigned int DataSeriesHydro::getIndex(unsigned int year) const
{
    return (count != 1) ? timeseriesNumbers[0][year] : 0;
}

} // namespace Data
} // namespace Antares
