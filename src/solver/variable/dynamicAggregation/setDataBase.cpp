// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/dynamicAggregation/setData.h"

namespace Antares::Solver::Variable
{

SetDataBase::SetDataBase(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    for (auto* area: set_)
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
