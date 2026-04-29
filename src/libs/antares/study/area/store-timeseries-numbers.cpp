// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/area/store-timeseries-numbers.h"

#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>

namespace fs = std::filesystem;

namespace Antares::Data
{
static void storeTSnumbers(Solver::IResultWriter& writer,
                           const TimeSeriesNumbers& timeseriesNumbers,
                           const std::string& id,
                           const fs::path& directory)
{
    std::string fullId = id + ".txt";
    fs::path path = "ts-numbers" / directory / fullId;

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer);

    writer.addEntryFromBuffer(path, buffer);
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesLoad>(Solver::IResultWriter& writer,
                                                                      const Area& area)
{
    storeTSnumbers(writer, area.load.series.timeseriesNumbers, area.id, "load");
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesSolar>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    storeTSnumbers(writer, area.solar.series.timeseriesNumbers, area.id, "solar");
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesHydro>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    storeTSnumbers(writer, area.hydro.series->timeseriesNumbers, area.id, "hydro");
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesWind>(Solver::IResultWriter& writer,
                                                                      const Area& area)
{
    storeTSnumbers(writer, area.wind.series.timeseriesNumbers, area.id, "wind");
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesThermal>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    area.thermal.list.storeTimeseriesNumbers(writer);
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesRenewable>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    area.renewable.list.storeTimeseriesNumbers(writer);
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesTransmissionCapacities>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    // No links originating from this area
    // do not create an empty directory
    if (area.links.empty())
    {
        return;
    }

    for (const auto& [key, value]: area.links)
    {
        if (value == nullptr)
        {
            logs.error() << "Unexpected nullptr encountered for area " << area.id;
            return;
        }
        else
        {
            value->storeTimeseriesNumbers(writer);
        }
    }
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesShortTermInflows>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    for (const auto& sts: area.shortTermStorage.storagesByIndex)
    {
        const auto area_sts = fs::path("st-storage") / area.id.c_str() / sts.id;
        storeTSnumbers(writer, sts.series->inflowsTSNumbers, "inflows", area_sts.string());
    }
}

template<>
void singleAreaStoreTimeseriesNumbers<TimeSeriesType::timeSeriesShortTermAdditionalConstraints>(
  Solver::IResultWriter& writer,
  const Area& area)
{
    for (const auto& sts: area.shortTermStorage.storagesByIndex)
    {
        for (const auto& ct: sts.additionalConstraints)
        {
            const auto area_sts = fs::path("st-storage") / area.id.c_str() / sts.id;
            storeTSnumbers(writer, ct->timeseriesNumbers, ct->name, area_sts.string());
        }
    }
}

} // namespace Antares::Data
