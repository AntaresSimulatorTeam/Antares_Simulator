// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


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
