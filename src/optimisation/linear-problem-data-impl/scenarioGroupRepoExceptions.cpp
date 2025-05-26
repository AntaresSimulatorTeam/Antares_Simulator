#include <string>

#include "antares/optimisation/linear-problem-data-impl/scenarioGroupRepo.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{

ScenarioGroupRepository::AlreadyExists::AlreadyExists(const std::string& groupId):
    std::invalid_argument("Scenario group '" + groupId + "' already exists in group repo.")
{
}

ScenarioGroupRepository::DoesNotExist::DoesNotExist(const std::string& groupId):
    std::invalid_argument("Scenario group '" + groupId + "' does not exist in group repo.")
{
}

ScenarioGroupRepository::ScenarioNotExist::ScenarioNotExist(const std::string& groupId,
                                                            const unsigned scenario):
    std::invalid_argument("In scenario group '" + groupId + "', scenario '"
                          + std::to_string(scenario) + "' does not exist.")
{
}

} // namespace Antares::Optimisation::LinearProblemDataImpl
