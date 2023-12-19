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

#include "../simulation/sim_extern_variables_globales.h"
#include "randomized-thermal-generator/RandomizedGenerator.h"
#include "optimized-thermal-generator/main/OptimizedGenerator.h"

namespace Antares::Solver::TSGenerator
{

bool GenerateRandomizedThermalTimeSeries(Data::Study& study,
                               uint year,
                               bool globalThermalTSgeneration,
                               bool refreshTSonCurrentYear,
                               Antares::Solver::IResultWriter& writer)
{
    logs.info();
    logs.info() << "Generating randomized thermal time-series";
    Solver::Progression::Task progression(study, year, Solver::Progression::sectTSGThermal);

    auto* generator = new GeneratorTempData(study, progression, writer);

    generator->currentYear = year;

    study.areas.each([&](Data::Area& area) {
        auto end = area.thermal.list.mapping.end();
        for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
        {
            auto& cluster = *(it->second);

            if (cluster.doWeGenerateTS(globalThermalTSgeneration) && refreshTSonCurrentYear)
            {
                (*generator)(area, cluster);
            }

            ++progression;
        }
    });

    delete generator;

    return true;
}

bool GenerateOptimizedThermalTimeSeries(Data::Study& study,
                                        uint year,
                                        bool globalThermalTSgeneration,
                                        Antares::Solver::IResultWriter& writer)
{
    // optimized planning should only be called once.
    // Due to possible thermal refresh span we can end up here more than once - but just ignore it
    // even if we set in Scenario playlist that year-1 should be skipped, we will execute this
    // properly
    if (year != 0)
        return true;

    logs.info();
    logs.info() << "Generating optimized thermal time-series";
    Solver::Progression::Task progression(study, year, Solver::Progression::sectTSGThermal);

    const auto& activeMaintenanceGroups = study.maintenanceGroups.activeMaintenanceGroups();
    for (const auto& entryMaintenanceGroup : activeMaintenanceGroups)
    {
        auto& maintenanceGroup = *(entryMaintenanceGroup.get());
        auto generator = OptimizedThermalGenerator(
          study, maintenanceGroup, year, globalThermalTSgeneration, progression, writer);
        generator.GenerateOptimizedThermalTimeSeries();
    }

    return true;
}

bool GenerateThermalTimeSeries(Data::Study& study,
                               uint year,
                               bool globalThermalTSgeneration,
                               bool refreshTSonCurrentYear,
                               Antares::Solver::IResultWriter& writer)
{
    if (study.parameters.maintenancePlanning.isOptimized())
    {
        return GenerateOptimizedThermalTimeSeries(study, year, globalThermalTSgeneration, writer);
    }
    else
    {
        return GenerateRandomizedThermalTimeSeries(
          study, year, globalThermalTSgeneration, refreshTSonCurrentYear, writer);
    }
}

} // namespace Antares::Solver::TSGenerator
