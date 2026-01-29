/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

namespace Antares::Solver::Variable
{

DynamicAggregationSingleYear::DynamicAggregationSingleYear(Data::Study& study):
    study_(study)
{
    for (const auto& set: study_.setsOfAreas)
    {
        setsData_.try_emplace(set.first, *set.second);
    }
}

void DynamicAggregationSingleYear::addResultsToSets(const PROBLEME_HEBDO& pb)
{
    for (auto& [_, setData]: setsData_)
    {
        setData.addResultsToSet(pb);
    }
}

void DynamicAggregationSingleYear::appendToSurveyForSet(const std::string& setName,
                                                        SurveyResults& survey,
                                                        Category::Precision precision) const
{
    auto it = setsData_.find(setName);
    if (it != setsData_.end())
    {
        it->second.appendToSurvey(survey, precision, study_);
    }
}

unsigned int computeDynamicAggregationMaxColumns(const Data::Study& study)
{
    using SetType = std::set<Data::Area*, Data::CompareAreaName>;

    unsigned int maxCols = 0;

    for (const auto& [setName, setPtr]: study.setsOfAreas)
    {
        const SetType& setAreas = *setPtr;

        std::set<std::string> thermalGroupNames;
        std::set<std::string> renewableGroupNames;
        std::set<std::string> stsGroupNames;

        for (auto* area: setAreas)
        {
            for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
            {
                thermalGroupNames.insert(cluster->getGroup());
            }

            for (const auto& cluster: area->renewable.list.each_enabled())
            {
                renewableGroupNames.insert(cluster->getGroup());
            }

            for (const auto& sts: area->shortTermStorage.storagesByIndex)
            {
                stsGroupNames.insert(sts.properties.groupName);
            }
        }

        unsigned int colsForSet = thermalGroupNames.size() + renewableGroupNames.size()
                                  + 3 * stsGroupNames.size();

        if (colsForSet > maxCols)
        {
            maxCols = colsForSet;
        }
    }

    return maxCols;
}

} // namespace Antares::Solver::Variable
