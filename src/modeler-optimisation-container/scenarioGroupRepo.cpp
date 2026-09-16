// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/modeler-optimisation-container/scenarioGroupRepo.h"

#include <boost/algorithm/string.hpp>

namespace Antares::LinearProblem
{
void ScenarioGroupRepository::addScenario(const std::string& groupId,
                                          std::unique_ptr<Api::IScenario>&& scenario)
{
    std::string gId = groupId;
    boost::to_upper(gId);
    if (scenarioGroups_.contains(gId))
    {
        throw AlreadyExists(gId);
    }
    scenarioGroups_[gId] = std::move(scenario);
}

class DefaultScenario final: public LinearProblem::Api::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year) const override
    {
        return 1; // Default rank for empty groupId
    }
};

const Api::IScenario& ScenarioGroupRepository::scenario(const std::string& groupId) const
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

bool ScenarioGroupRepository::contains(const std::string& groupId) const
{
    if (groupId.empty())
    {
        return false;
    }
    std::string gId = groupId;
    boost::to_upper(gId);
    return scenarioGroups_.contains(gId);
}
} // namespace Antares::LinearProblem
