#include "antares/optimisation/linear-problem-data-impl/scenarioGroupRepo.h"

#include <stdexcept>

namespace Antares::Optimisation::LinearProblemDataImpl
{
void ScenarioGroupRepository::addPairScenarioRankToGroup(
  const std::string& groupId,
  const std::pair<unsigned, unsigned> scenarioToRank)
{
    if (scenarioGroups_.contains(groupId))
    {
        throw AlreadyExists(groupId);
    }
    scenarioGroups_[groupId] = {scenarioToRank};
}

unsigned ScenarioGroupRepository::getDataRank(const std::string& groupId, const unsigned scenario)
{
    if (!scenarioGroups_.contains(groupId))
    {
        throw DoesNotExist(groupId);
    }

    if (!scenarioGroups_.at(groupId).contains(scenario))
    {
        throw ScenarioNotExist(groupId, scenario);
    }

    return scenarioGroups_.at(groupId).at(scenario);
}
} // namespace Antares::Optimisation::LinearProblemDataImpl
