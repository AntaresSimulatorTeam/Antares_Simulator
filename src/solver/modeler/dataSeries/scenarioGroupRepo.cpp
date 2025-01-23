#include "antares/solver/modeler/dataSeries/scenarioGroupRepo.h"

#include <stdexcept>

#include "antares/solver/modeler/dataSeries/scenarioGroupRepoExceptions.h"

namespace Antares::Solver::Modeler::DataSeries
{
void ScenarioGroupRepository::addPairScenarioRankToGroup(
  const std::string& groupId,
  const std::pair<unsigned, unsigned> scenarioToRank)
{
    scenarioGroups_[groupId] = {scenarioToRank};
}

unsigned ScenarioGroupRepository::getDataRank(const std::string& groupId, const unsigned scenario)
{
    if (!scenarioGroups_.contains(groupId))
    {
        throw ScGroup_DoesNotExist(groupId);
    }

    if (!scenarioGroups_.at(groupId).contains(scenario))
    {
        throw ScGroup_ScenarioNotExist(groupId, scenario);
    }

    return scenarioGroups_.at(groupId).at(scenario);
}
} // namespace Antares::Solver::Modeler::DataSeries
