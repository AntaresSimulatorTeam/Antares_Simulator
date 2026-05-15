// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/thermal/ecoInput.h"

#include <filesystem>

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;

namespace fs = std::filesystem;

#define SEP IO::Separator

namespace Antares::Data
{
EconomicInputData::EconomicInputData() = default;

bool EconomicInputData::loadFromFolder(Study& study, const fs::path& folder)
{
    bool ret = true;

    if (study.header.version >= StudyVersion(8, 7))
    {
        Yuni::Clob dataBuffer;

        fs::path filename = folder / "fuelCost.txt";
        if (fs::exists(filename))
        {
            ret = fuelcost.loadFromCSVFile(filename.string(),
                                           1,
                                           HOURS_PER_YEAR,
                                           Matrix<>::optImmediate,
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
                                          Matrix<>::optImmediate,
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
