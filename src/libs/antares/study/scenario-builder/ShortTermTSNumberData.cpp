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
    return 0;
}

static bool ApplyToAdditionalConstraintsRhs(

  const Antares::Data::Area* area,
  const std::string& cluster_id,
  ShortTermStorage::AdditionalConstraints& additionalConstraints,
  const Matrix<uint32_t>& col,
  uint tsGenMax)
{
    // Errors
    uint errors = 0;
    CString<512, false> logprefix;
    logprefix.clear() << "Short Term Storage: Area '" << area->name << "', cluster '" << cluster_id
                      << "', constraint '" << additionalConstraints.name << "' :";

    return ApplyToMatrix(errors, logprefix, additionalConstraints.series(), col[0], tsGenMax);
}

static bool ApplyToClusterInflows(const Antares::Data::Area* area,
                                  ShortTermStorage::STStorageCluster& cluster,
                                  const Matrix<uint32_t>& col,
                                  uint tsGenMax)
{
    // Errors
    uint errors = 0;
    CString<512, false> logprefix;
    logprefix.clear() << "Short Term Storage: Area '" << area->name << "', cluster '" << cluster.id
                      << "' inflows:";
    return ApplyToMatrix(errors, logprefix, cluster.series->inflows.series, col[0], tsGenMax);
}

bool ShortTermTSNumberData::apply(Study& study)
{
    bool ret = true;
    auto tsGenMax = get_tsGenCount(study);
    for (const auto& area: study.areas | std::views::values)
    {
        for (auto& cluster: area->shortTermStorage.storagesByIndex)
        {
            Matrix<uint32_t>& mapped = rules_.at(
              ShortTermTSNumberData::key{area->id.c_str(), cluster.id});
            // const MatrixType::ColumnType& col = mapped[0];
            ret = ApplyToClusterInflows(area, cluster, mapped, tsGenMax) && ret;
            for (auto& additionalConstraints: cluster.additionalConstraints)
            {
                if (additionalConstraints.enabled)
                {
                    ret = ApplyToAdditionalConstraintsRhs(area,
                                                          cluster.id,
                                                          additionalConstraints,
                                                          mapped,
                                                          tsGenMax)
                          && ret;
                }
            }
        }
    }
    return ret;
}

void ShortTermTSNumberData::setTSnumber(const std::string& area_name,
                                        const std::string& cluster_name,
                                        unsigned year,
                                        unsigned value)
{
    if (auto& ts_numbers = rules_.at(ShortTermTSNumberData::key{area_name, cluster_name});
        year < ts_numbers.height)
    {
        ts_numbers[0][year] = value;
    }
}

unsigned ShortTermTSNumberData::get_value(const std::string& area_name,
                                          const std::string& cluster_name,
                                          unsigned year) const
{
    // TODO check
    return rules_.at(ShortTermTSNumberData::key{area_name, cluster_name})[0][year];
}

size_t ShortTermTSNumberData::keyHasher::operator()(const key& k) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, k.area_name);
    boost::hash_combine(seed, k.cluster_name);
    // boost::hash_combine(seed, k.year);
    return seed;
}

bool ShortTermTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    for (const auto& area: study.areas | std::views::values)
    {
        for (const auto& cluster: area->shortTermStorage.storagesByIndex)
        {
            auto& ts_numbers = rules_[ShortTermTSNumberData::key{area->id.c_str(), cluster.id}];
            ts_numbers.reset(1, nbYears);
        }
    }
    return true;
}
} // namespace Antares::Data::ScenarioBuilder
