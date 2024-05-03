/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include "antares/series/series.h"
#include <algorithm>
#include <vector>

using namespace Yuni;

#define SEP IO::Separator


namespace Antares::Data
{
void TimeSeries::Numbers::registerSeries(const TimeSeries* s, std::string label)
{
    series.push_back(s);
}

// TODO[FOM] Code duplication
static bool checkAllElementsIdenticalOrOne(std::vector<uint> w)
{
    auto first_one = std::remove(w.begin(), w.end(), 1); // Reject all 1 to the end
    return std::adjacent_find(w.begin(), first_one, std::not_equal_to<uint>()) == first_one;
}

bool TimeSeries::Numbers::checkSeriesNumberOfColumnsConsistency() const
{
    std::vector<uint> width;
    for (const auto* s : series)
        width.push_back(s->numberOfColumns());

    return checkAllElementsIdenticalOrOne(width);
}

TimeSeries::TimeSeries(Numbers& tsNumbers, std::string label) : timeseriesNumbers(tsNumbers)
{
    tsNumbers.registerSeries(this, std::move(label));
}

bool TimeSeries::loadFromFile(const std::string& path,
                              const bool average)
{
    bool ret = true;
    Matrix<>::BufferType dataBuffer;
    ret = timeSeries.loadFromCSVFile(path, 1, HOURS_PER_YEAR, &dataBuffer) && ret;

    if (average)
        timeSeries.averageTimeseries();

    timeseriesNumbers.clear();

    return ret;
}

int TimeSeries::saveToFolder(const AreaName& areaID,
                             const std::string& folder,
                             const std::string& prefix) const
{
    Clob buffer;
    buffer.clear() << folder << SEP << prefix << areaID << ".txt";
    return timeSeries.saveToCSVFile(buffer, 0);
}

int TimeSeries::saveToFile(const std::string& filename, bool saveEvenIfAllZero) const
{
    return timeSeries.saveToCSVFile(filename, 6, false, saveEvenIfAllZero);
}

double TimeSeries::getCoefficient(uint32_t year, uint32_t timestep) const
{
    return timeSeries[getSeriesIndex(year)][timestep];
}

const double* TimeSeries::getColumn(uint32_t year) const
{
    return timeSeries[getSeriesIndex(year)];
}

uint32_t TimeSeries::getSeriesIndex(uint32_t year) const
{
    // If the timeSeries only has one column, we have no choice but to use it.
    if (numberOfColumns() == 1)
        return 0;

    return timeseriesNumbers[0][year];
}

double* TimeSeries::operator[](uint32_t index)
{
    if (timeSeries.width <= index)
        return nullptr;
    return timeSeries[index];
}

void TimeSeries::reset()
{
    timeSeries.reset(1, HOURS_PER_YEAR);
}

void TimeSeries::reset(uint32_t width, uint32_t height)
{
    timeSeries.reset(width, height);
}

uint32_t TimeSeries::numberOfColumns() const
{
    return timeSeries.width;
}

void TimeSeries::resize(uint32_t timeSeriesCount, uint32_t timestepCount)
{
    timeSeries.resize(timeSeriesCount, timestepCount);
}

void TimeSeries::fill(double value)
{
    timeSeries.fill(value);
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
