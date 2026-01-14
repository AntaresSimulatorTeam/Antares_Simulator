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

#include "antares/solver/variable/storage/averagedata.h"

namespace Antares::Solver::Variable
{

void DynamicAggregation::initializeStorage(const Data::Study& study)
{
    // for each set do the following

    std::map<std::string, int> nameAndNumberOfOccurrences;
    for (auto& area: study.setsOfAreas[0])
    {
        // handle sts later
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();
            nameAndNumberOfOccurrences[groupName]++;
        }
    }

    std::vector<std::string> groupNames;
    for (const auto& nameAndNum: nameAndNumberOfOccurrences)
    {
        groupNames.push_back(nameAndNum.first);
    }
    std::map<std::string, unsigned int> groupToNumbers = Utils::giveNumbersToStrings(groupNames);
    unsigned int nbColumns = groupNames.size();

    std::vector<R::AllYears::AverageData> results;
    for (unsigned int i = 0; i < nbColumns; i++)
    {
        results.push_back(R::AllYears::AverageData(nameAndNumberOfOccurrences[groupNames[i]]));
    }
}

} // namespace Antares::Solver::Variable
