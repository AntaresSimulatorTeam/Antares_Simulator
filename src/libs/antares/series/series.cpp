// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/series/series.h"

#include <functional>
#include <filesystem>
#include <sstream>
#include <vector>

#include <antares/antares/constants.h>
#include <antares/array/matrix-io.h>
#include <antares/utils/utils.h>

namespace Antares::Data
{
const std::vector<double> TimeSeries::emptyColumn{};

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
    return static_cast<uint>(tsNumbers.size());
}

uint32_t TimeSeriesNumbers::operator[](uint y) const
{
    return tsNumbers[y];
}

uint32_t& TimeSeriesNumbers::operator[](uint y)
{
    return tsNumbers[y];
}

void TimeSeriesNumbers::reset(uint h)
{
    tsNumbers.assign(h, 0);
}

void TimeSeriesNumbers::clear()
{
    tsNumbers.clear();
}

void TimeSeriesNumbers::saveToBuffer(std::string& data) const
{
    Matrix matrix(1, height());
    for (unsigned int y = 0; y < height(); ++y)
    {
        matrix[0][y] = tsNumbers[y] + 1;
    }
    MatrixIO::saveToBuffer(matrix, data, 0, false, std::identity{}, true);
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

bool TimeSeries::loadFromFile(const std::filesystem::path& path, const bool average)
{
    bool ret = true;
    ret = MatrixIO::load(timeSeries, path.string(), 1, HOURS_PER_YEAR) && ret;

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
    const std::filesystem::path path = std::filesystem::path(folder) / (prefix + areaID + ".txt");
    return MatrixIO::save(timeSeries, path.string(), 0);
}

double TimeSeries::getCoefficient(uint32_t year, uint32_t timestep) const
{
    return timeSeries[getSeriesIndex(year)][timestep];
}

const double* TimeSeries::getColumn(uint32_t year) const
{
    if (timeSeries.width() == 0)
    {
        return emptyColumn.data();
    }
    return timeSeries[getSeriesIndex(year)].data();
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

Matrix::ColumnType& TimeSeries::operator[](uint32_t index)
{
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
    return timeSeries.width();
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

} // namespace Antares::Data
