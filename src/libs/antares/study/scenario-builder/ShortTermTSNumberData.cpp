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

#include "antares/study/scenario-builder/ShortTermTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint ShortTermTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    const bool tsGenSt = (0 != (parameters.timeSeriesToGenerate & timeSeriesShortTermStorage));
    return tsGenSt ? parameters.nbTimeSeriesShortTermStorage : 0u;
}

bool ShortTermTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    for (const auto& area: study.areas | std::views::values)
    {
        for (const auto& cluster: area->shortTermStorage.storagesByIndex)
        {
            for (const auto& additionalConstraints: cluster.additionalConstraints)
            {
                if (additionalConstraints.enabled)
                {
                    auto& ts_numbers = rules_[ShortTermTSNumberData::key{
                      area->name,
                      cluster.id,
                      additionalConstraints.name}];
                    logprefix.clear()
                      << "Short Term Storage: Area '" << area->name << "', cluster '" << cluster.id
                      << "', constraint '" << additionalConstraints.name << "' :";
                    const MatrixType::ColumnType& col = rules_.at(
                      ShortTermTSNumberData::key{area->name,
                                                 cluster.id,
                                                 additionalConstraints.name})[0];
                    ret = ApplyToMatrix(errors,
                                        logprefix,
                                        additionalConstraints.series,
                                        col,
                                        get_tsGenCount(study))
                          && ret;
                }
            }
        }
    }
    return ret;
}

void ShortTermTSNumberData::setTSnumber(const std::string& area_name,
                                        const std::string& cluster_name,
                                        const std::string& constraintName,
                                        unsigned year,
                                        unsigned value)
{
    if (auto& ts_numbers = rules_[ShortTermTSNumberData::key{area_name,
                                                             cluster_name,
                                                             constraintName}];
        year < ts_numbers.height)
    {
        ts_numbers[0][year] = value;
    }
}

unsigned ShortTermTSNumberData::get_value(const std::string& area_name,
                                          const std::string& cluster_name,
                                          const std::string& constraintName,
                                          unsigned year) const
{
    // TODO check
    return rules_.at(ShortTermTSNumberData::key{area_name, cluster_name, constraintName})[0][year];
}

bool ShortTermTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    for (const auto& area: study.areas | std::views::values)
    {
        for (const auto& cluster: area->shortTermStorage.storagesByIndex)
        {
            for (const auto& additionalConstraints: cluster.additionalConstraints)
            {
                auto& ts_numbers = rules_[ShortTermTSNumberData::key{area->name,
                                                                     cluster.id,
                                                                     additionalConstraints.name}];
                ts_numbers.reset(1, nbYears);
            }
        }
    }
    return true;
}
} // namespace Antares::Data::ScenarioBuilder
