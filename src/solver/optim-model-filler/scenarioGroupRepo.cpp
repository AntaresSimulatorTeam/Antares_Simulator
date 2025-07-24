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

#include "antares/solver/optim-model-filler/scenarioGroupRepo.h"

namespace Antares::Optimisation
{
void ScenarioGroupRepository::addScenario(const std::string& groupId,
                                          std::unique_ptr<LinearProblemApi::IScenario>&& scenario)
{
    if (scenarioGroups_.contains(groupId))
    {
        throw AlreadyExists(groupId);
    }
    scenarioGroups_[groupId] = std::move(scenario);
}

class DefaultScenario: public Optimisation::LinearProblemApi::IScenario
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
    // A component require a group id. Assuming that the default group id is "default"
    if (groupId.empty() || groupId == "default")
    {
        static DefaultScenario defaultScenario(
          "default");           // Todo: default ou empty for consistency ?
        return defaultScenario; // Default rank for empty groupId
    }
    if (!scenarioGroups_.contains(groupId))
    {
        throw DoesNotExist(groupId);
    }
    return *scenarioGroups_.at(groupId);
}
} // namespace Antares::Optimisation
