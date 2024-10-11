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

void EconomicInputData::copyFrom(const EconomicInputData& rhs)
{
    fuelcost = rhs.fuelcost;
    rhs.fuelcost.unloadFromMemory();
    co2cost = rhs.co2cost;
    rhs.co2cost.unloadFromMemory();
}

bool EconomicInputData::saveToFolder(const AnyString& folder) const
{
    bool ret = true;
    if (IO::Directory::Create(folder))
    {
        String buffer;
        buffer.clear() << folder << SEP << "fuelCost.txt";
        ret = fuelcost.saveToCSVFile(buffer) && ret;
        buffer.clear() << folder << SEP << "CO2Cost.txt";
        ret = co2cost.saveToCSVFile(buffer) && ret;
        return ret;
    }
    return false;
}

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
            if (study.usedByTheSolver && study.parameters.derated)
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
            if (study.usedByTheSolver && study.parameters.derated)
            {
                co2cost.averageTimeseries();
            }
        }
    }

    return ret;
}

bool EconomicInputData::forceReload(bool reload) const
{
    return fuelcost.forceReload(reload) && co2cost.forceReload(reload);
}

void EconomicInputData::markAsModified() const
{
    fuelcost.markAsModified();
    co2cost.markAsModified();
}

void EconomicInputData::reset()
{
    fuelcost.reset(1, HOURS_PER_YEAR, true);
    co2cost.reset(1, HOURS_PER_YEAR, true);
}

} // namespace Antares::Data
