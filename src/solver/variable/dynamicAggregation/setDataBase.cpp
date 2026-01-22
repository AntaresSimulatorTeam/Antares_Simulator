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

SetDataBase::SetDataBase(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    for (auto& area: set_)
    {
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            thermalGroupNames_.insert(cluster->getGroup());
        }
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            renewableGroupNames_.insert(cluster->getGroup());
        }
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            stsGroupNames_.insert(sts.properties.groupName);
        }
    }

    thermalGroupToNumbers_ = Utils::giveNumbersToStrings(thermalGroupNames_);
    renewableGroupToNumbers_ = Utils::giveNumbersToStrings(renewableGroupNames_);
    stsGroupToNumbers_ = Utils::giveNumbersToStrings(stsGroupNames_);
}

} // namespace Antares::Solver::Variable