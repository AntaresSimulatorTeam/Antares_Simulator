// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/ecoInput.h"

#include <filesystem>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

namespace fs = std::filesystem;

#define SEP Yuni::IO::Separator

namespace Antares::Data
{
EconomicInputData::EconomicInputData() = default;

bool EconomicInputData::loadFromFolder(Study& study, const fs::path& folder)
{
    bool ret = true;

    if (study.header.version >= StudyVersion(8, 7))
    {
        Matrix<>::BufferType dataBuffer;

        fs::path filename = folder / "fuelCost.txt";
        if (fs::exists(filename))
        {
            ret = fuelcost.loadFromCSVFile(filename.string(),
                                           1,
                                           HOURS_PER_YEAR,
                                           Matrix<>::optNone,
                                           &dataBuffer)
                  && ret;
            if (study.parameters.derated)
            {
                fuelcost.averageTimeseries();
            }
        }

        filename = folder / "CO2Cost.txt";
        if (fs::exists(filename))
        {
            ret = co2cost.loadFromCSVFile(filename.string(),
                                          1,
                                          HOURS_PER_YEAR,
                                          Matrix<>::optNone,
                                          &dataBuffer)
                  && ret;
            if (study.parameters.derated)
            {
                co2cost.averageTimeseries();
            }
        }
    }

    return ret;
}

} // namespace Antares::Data
