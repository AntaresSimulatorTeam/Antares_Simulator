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
//
// Created by marechaljas on 04/07/23.
//

#include "antares/study/scenario-builder/ThermalTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool thermalTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // If an area is available, it can only be an overlay for thermal timeseries
    // WARNING: The total number of clusters may vary if used from the
    //   solver or not.
    // WARNING: At this point, the variable pArea->thermal.list.size()
    //   might not be valid (because not really initialized yet)
    uint clusterCount = pArea->thermal.list.allClustersCount();

    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}

void thermalTSNumberData::saveToINIFile(const Study& /* study */,
                                        Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
    {
        return;
    }

    for (auto& cluster: pArea->thermal.list.all())
    {
        for (uint year = 0; year != pTSNumberRules.height; ++year)
        {
            const uint val = get(cluster.get(), year);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
            {
                continue;
            }
            file << prefix << pArea->id << "," << year << ',' << cluster->id() << " = " << val
                 << '\n';
        }
    }
}

void thermalTSNumberData::setTSnumber(const Antares::Data::ThermalCluster* cluster,
                                      const uint year,
                                      uint value)
{
    assert(cluster != nullptr);
    if (year < pTSNumberRules.height && cluster->areaWideIndex < pTSNumberRules.width)
    {
        pTSNumberRules[cluster->areaWideIndex][year] = value;
    }
}

bool thermalTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    const Area& area = *(study.areas.byIndex[pArea->index]);

    for (auto& cluster: area.thermal.list.each_enabled())
    {
        assert(cluster->areaWideIndex < pTSNumberRules.width);
        const auto& col = pTSNumberRules[cluster->areaWideIndex];

        uint tsGenCount = cluster->tsGenBehavior == LocalTSGenerationBehavior::forceNoGen
                            ? cluster->series.timeSeries.width
                            : get_tsGenCount(study);

        logprefix.clear() << "Thermal: area '" << area.name << "', cluster: '" << cluster->name()
                          << "': ";
        ret = ApplyToMatrix(errors, logprefix, cluster->series, col, tsGenCount) && ret;
    }
    return ret;
}

uint thermalTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    bool tsGenThermal = (0 != (parameters.timeSeriesToGenerate & timeSeriesThermal));
    return tsGenThermal ? parameters.nbTimeSeriesThermal : 0u;
}
} // namespace Antares::Data::ScenarioBuilder
