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

    writer.addEntryFromBuffer(path.string(), buffer);
}

void storeTimeseriesNumbersForLoad(Solver::IResultWriter& writer, const Area& area)
{
    storeTSnumbers(writer, area.load.series.timeseriesNumbers, area.id, "load");
}

void storeTimeseriesNumbersForSolar(Solver::IResultWriter& writer, const Area& area)
{
    storeTSnumbers(writer, area.solar.series.timeseriesNumbers, area.id, "solar");
}

void storeTimeseriesNumbersForHydro(Solver::IResultWriter& writer, const Area& area)
{
    storeTSnumbers(writer, area.hydro.series->timeseriesNumbers, area.id, "hydro");
}

void storeTimeseriesNumbersForWind(Solver::IResultWriter& writer, const Area& area)
{
    storeTSnumbers(writer, area.wind.series.timeseriesNumbers, area.id, "wind");
}

void storeTimeseriesNumbersForThermal(Solver::IResultWriter& writer, const Area& area)
{
    area.thermal.list.storeTimeseriesNumbers(writer);
}

void storeTimeseriesNumbersForRenewable(Solver::IResultWriter& writer, const Area& area)
{
    area.renewable.list.storeTimeseriesNumbers(writer);
}

void storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter& writer,
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
} // namespace Antares::Data
