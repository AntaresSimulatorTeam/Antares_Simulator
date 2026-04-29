// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/series/series.h"

#include <sstream>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/antares/constants.h>
#include <antares/utils/utils.h>

namespace Antares::Data
{
void TimeSeriesNumbers::registerSeries(const TimeSeries* s, std::string label)
{
    series[std::move(label)] = s;
}

static std::string errorMessage(const std::map<std::string, const TimeSeries*>& series)
{
    std::ostringstream msg;
    auto isLast = [&series](std::size_t& idx)
    {
        idx++;
        return idx == series.size();
    };
    for (std::size_t idx = 0; const auto& [label, s]: series)
    {
        msg << label << ": " << s->numberOfColumns() << (isLast(idx) ? "" : ", ");
    }
    return msg.str();
}

uint TimeSeriesNumbers::height() const
{
    return tsNumbers.height;
}

uint32_t TimeSeriesNumbers::operator[](uint y) const
{
    return tsNumbers[0][y];
}

uint32_t& TimeSeriesNumbers::operator[](uint y)
{
    return tsNumbers[0][y];
}

void TimeSeriesNumbers::reset(uint h)
{
    tsNumbers.reset(1, h);
}

void TimeSeriesNumbers::clear()
{
    tsNumbers.clear();
}

void TimeSeriesNumbers::saveToBuffer(std::string& data) const
{
    const auto add1 = [](uint32_t x) { return x + 1; };
    tsNumbers.saveToBuffer(data, 0, true, add1, true);
}

std::optional<std::string> TimeSeriesNumbers::checkSeriesNumberOfColumnsConsistency() const
{
    std::vector<uint> width;
    for (const auto& [_, s]: series)
    {
        width.push_back(s->numberOfColumns());
    }

    if (!Utils::checkAllElementsIdenticalOrOne(width))
    {
        return errorMessage(series);
    }
    return std::nullopt;
}

TimeSeries::TimeSeries(TimeSeriesNumbers& tsNumbers):
    timeseriesNumbers(tsNumbers)
{
}

bool TimeSeries::loadFromFile(const std::filesystem::path& path,
                              const bool average,
                              unsigned options)
{
    bool ret = true;
    Matrix<>::BufferType dataBuffer;
    ret = timeSeries.loadFromCSVFile(path.string(), 1, HOURS_PER_YEAR, options, &dataBuffer) && ret;

    if (average)
    {
        timeSeries.averageTimeseries();
    }

    return ret;
}

int TimeSeries::saveToFolder(const std::string& areaID,
                             const std::string& folder,
                             const std::string& prefix) const
{
    Yuni::Clob buffer;
    buffer.clear() << folder << Yuni::IO::Separator << prefix << areaID << ".txt";
    return timeSeries.saveToCSVFile(buffer, 0);
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
    {
        return 0;
    }

    return timeseriesNumbers[year];
}

double* TimeSeries::operator[](uint32_t index)
{
    if (timeSeries.width <= index)
    {
        return nullptr;
    }
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

} // namespace Antares::Data
