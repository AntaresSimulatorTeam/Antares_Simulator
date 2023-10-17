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
#include <yuni/io/directory.h>
#include "include/antares/series/series.h"

using namespace Yuni;

#define SEP IO::Separator


namespace Antares::Data
{

const double TimeSeries::emptyColumn[] = {0};

TimeSeries::TimeSeries(TSNumbers& tsNumbers) : timeseriesNumbers(tsNumbers)
{}

bool TimeSeries::timeSeriesLoadFromFolder(const std::string& path,
                                          Matrix<>::BufferType dataBuffer,
                                          const bool average)
{
    bool ret = 1;
    ret = timeSeries.loadFromCSVFile(path, 1, HOURS_PER_YEAR, &dataBuffer) && ret;

    if (average)
        timeSeries.averageTimeseries();

    timeseriesNumbers.clear();

    return ret;
}

int TimeSeries::timeSeriesSaveToFolder(const AreaName& areaID, const std::string& folder,
                                       const std::string& prefix) const
{
    Clob buffer;
    buffer.clear() << folder << SEP << prefix << areaID << ".txt";
    return timeSeries.saveToCSVFile(buffer, 0);
}



double TimeSeries::getCoefficient(uint32_t year, uint32_t hourInYear) const
{
    if (timeSeries.width == 0)
        return 0;
    return timeSeries[getSeriesIndex(year)][hourInYear];
}

const double* TimeSeries::getColumn(uint32_t year) const
{
    if (timeSeries.width == 0)
        return emptyColumn;
    return timeSeries[getSeriesIndex(year)];
}

uint32_t TimeSeries::getSeriesIndex(uint32_t year) const
{
    if (timeSeries.width == 1)
        return 0;
    else
        return timeseriesNumbers[0][year];
}

double* TimeSeries::operator[](uint32_t year)
{
    if (timeSeries.width == 0)
        return nullptr;
    return timeSeries[getSeriesIndex(year)];
}

void TimeSeries::reset()
{
    timeSeries.reset(1, HOURS_PER_YEAR);
}

void TimeSeries::resize(uint32_t year, uint32_t hour)
{
    timeSeries.resize(year, hour);
}

void TimeSeries::roundAllEntries()
{
    timeSeries.roundAllEntries();
}

void TimeSeries::averageTimeseries()
{
    timeSeries.averageTimeseries();
}

void TimeSeries::unloadFromMemory() const
{
    timeSeries.unloadFromMemory();
}

bool TimeSeries::forceReload(bool reload) const
{
    return timeSeries.forceReload(reload);
}

void TimeSeries::markAsModified() const
{
    timeSeries.markAsModified();
}

uint64_t TimeSeries::memoryUsage() const
{
    return timeSeries.memoryUsage();
}

} // namespace Antares::Data
