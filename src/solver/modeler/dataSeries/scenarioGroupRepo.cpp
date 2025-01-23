#include "include/antares/solver/modeler/dataSeries/scenarioGroupRepo.h"

#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
void ScenarioGroupRepository::addPairScenarioRankToGroup(
  const std::string groupId,
  const std::pair<unsigned, unsigned> scenarioToRank)
{
    scenarioGroups_[groupId] = {scenarioToRank};
}

unsigned ScenarioGroupRepository::getDataRank(const std::string groupId, const unsigned scenario)
{
    if (!scenarioGroups_.contains(groupId))
    {
        std::string error_message = "Group '" + groupId + "' does not exist in group repo.";
        throw std::invalid_argument(error_message);
    }

    if (!scenarioGroups_.at(groupId).contains(scenario))
    {
        std::string error_message = "In scenario group '" + groupId + "', scenario '"
                                    + std::to_string(scenario) + "' does not exist.";
        throw std::invalid_argument(error_message);
    }

    return scenarioGroups_.at(groupId).at(scenario);
}
} // namespace Antares::Solver::Modeler::DataSeries
