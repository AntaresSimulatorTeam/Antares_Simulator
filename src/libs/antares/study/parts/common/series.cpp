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
#include "../../study.h"
#include "series.h"

using namespace Yuni;

#define SEP IO::Separator


namespace Antares::Data
{

int TimeSeries::timeSeriesLoadFromFolder(Study& s,
        const AreaName& areaID,
        const std::string& folder,
        const std::string& filename)
{
    String& buffer = s.bufferLoadingTS;

    int ret = 1;
    buffer.clear() << folder << SEP << filename << areaID << '.' << s.inputExtension;
    ret = timeSeries.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, &s.dataBuffer) && ret;

    if (s.usedByTheSolver && s.parameters.derated)
        timeSeries.averageTimeseries();

    timeseriesNumbers.clear();

    return ret;
}

int TimeSeries::timeSeriesSaveToFolder(const AreaName& areaID, const std::string& folder,
                                       const std::string& filename) const
{
    Clob buffer;
    int ret = 1;
    buffer.clear() << folder << SEP << filename << areaID << ".txt";
    ret = timeSeries.saveToCSVFile(buffer, 0) && ret;

    return ret;
}

void TimeSeries::reset()
{
    timeSeries.reset(1, HOURS_PER_YEAR);
}

double TimeSeries::getCoefficient(uint32_t year, uint32_t hourInYear) const
{
    return timeSeries[getSeriesIndex(year)][hourInYear];
}

double* TimeSeries::getColumn(uint32_t year) const
{
    return timeSeries[getSeriesIndex(year)];
}

uint32_t TimeSeries::getSeriesIndex(uint32_t year) const
{
    if (timeSeries.width == 1)
        return 0;
    else
        return timeseriesNumbers[0][year];
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

bool DataSeriesCommon::forceReload(bool reload) const
{
    return timeSeries.forceReload(reload);
}

void DataSeriesCommon::markAsModified() const
{
    timeSeries.markAsModified();
}

uint64_t DataSeriesCommon::memoryUsage() const
{
    return timeSeries.memoryUsage();
}

double DataSeriesCommon::getAvailablePower(unsigned int hour, unsigned int year) const
{
    return timeSeries[getSeriesIndex(year)][hour];
}

const DataSeriesCommon::SingleYear& DataSeriesCommon::getAvailablePowerYearly(unsigned int year) const
{
    return timeSeries[getSeriesIndex(year)];
}

uint DataSeriesCommon::getSeriesIndex(unsigned int year) const
{
    if (timeSeries.width == 1)
        return 0;
    else
        return timeseriesNumbers[0][year];
}

} // namespace Antares::Data

