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

#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"

#include <boost/algorithm/string.hpp>

namespace Antares::Optimisation
{
void ScenarioGroupRepository::addScenario(const std::string& groupId,
                                          std::unique_ptr<LinearProblemApi::IScenario>&& scenario)
{
    std::string gId = groupId;
    boost::to_upper(gId);
    if (scenarioGroups_.contains(gId))
    {
        throw AlreadyExists(gId);
    }
    scenarioGroups_[gId] = std::move(scenario);
}

class DefaultScenario final: public Optimisation::LinearProblemApi::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

const LinearProblemApi::IScenario& ScenarioGroupRepository::scenario(
  const std::string& groupId) const
{
    // A component requires a group ID
    if (groupId.empty())
    {
        const static DefaultScenario defaultScenario("");
        return defaultScenario; // Default rank for empty groupId
    }

    std::string gId = groupId;
    boost::to_upper(gId);
    if (!scenarioGroups_.contains(gId))
    {
        throw DoesNotExist(gId);
    }
    return *scenarioGroups_.at(gId);
}
} // namespace Antares::Optimisation
