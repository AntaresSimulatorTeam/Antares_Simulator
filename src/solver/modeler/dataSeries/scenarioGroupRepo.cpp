#include "include/antares/solver/modeler/dataSeries/scenarioGroupRepo.h"

#include <format>
#include <stdexcept>

namespace Antares::Solver::Modeler::DataSeries
{
void ScenarioGroupRepository::addPairScenarioRankToGroup(
  std::string groupId,
  std::pair<unsigned, unsigned> scenarioToRank)
{
    scenarioGroups_[groupId] = {scenarioToRank};
}

unsigned ScenarioGroupRepository::getDataRank(std::string groupId, unsigned scenario)
{
    if (!scenarioGroups_.contains(groupId))
    {
        std::string error_message = "Group '" + groupId + "' does not exist in group repo.";
        throw std::invalid_argument(error_message);
    }

    if (!scenarioGroups_.at(groupId).contains(scenario))
    {
        std::string error_message = std::format("In scenario group '{}', scenario '{}' does not exist.",
                                                groupId,
                                                scenario);
        throw std::invalid_argument(error_message);
    }

    return scenarioGroups_.at(groupId).at(scenario);
}
} // namespace Antares::Solver::Modeler::DataSeries
