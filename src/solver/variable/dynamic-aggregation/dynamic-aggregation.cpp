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

#include "antares/solver/variable/dynamic-aggregation/dynamic-aggregation.h"

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable
{

SetData::SetData(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    std::map<std::string, int> nameAndNumberOfOccurrences; // only used to size results_
    for (auto& area: set_)
    {
        // handle sts later
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();
            nameAndNumberOfOccurrences[groupName]++;
        }
    }

    for (const auto& nameAndNum: nameAndNumberOfOccurrences)
    {
        groupNames_.push_back(nameAndNum.first);
    }
    groupToNumbers_ = Utils::giveNumbersToStrings(groupNames_);
    unsigned int nbColumns = groupNames_.size();

    for (unsigned int i = 0; i < nbColumns; i++)
    {
        results_.push_back(R::AllYears::AverageData(nameAndNumberOfOccurrences[groupNames_[i]]));
    }
}

void SetData::mergeValues(const std::string& groupName, const std::vector<double>& values)
{
    unsigned index = groupToNumbers_[groupName];

    IntermediateValues valuesForAllGranularities;
    std::copy(values.begin(), values.end(), valuesForAllGranularities.hour);

    valuesForAllGranularities.computeAveragesForCurrentYearFromHourlyResults();

    results_[index].merge(valuesForAllGranularities);
}

void DynamicAggregation::initializeSetsData(const Data::Study& study)
{
    for (const auto& set: study.setsOfAreas)
    {
        setsData_.emplace_back(*set.second);
    }
}

} // namespace Antares::Solver::Variable
