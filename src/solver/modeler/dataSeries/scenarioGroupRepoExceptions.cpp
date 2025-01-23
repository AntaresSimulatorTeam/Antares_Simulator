#include "antares/solver/modeler/dataSeries/scenarioGroupRepoExceptions.h"

#include <string>

namespace Antares::Solver::Modeler::DataSeries
{

ScGroup_DoesNotExist::ScGroup_DoesNotExist(const std::string& groupId):
    std::invalid_argument("Scenario group '" + groupId + "' does not exist in group repo.")
{
}

ScGroup_ScenarioNotExist::ScGroup_ScenarioNotExist(const std::string& groupId,
                                                   const unsigned scenario):
    std::invalid_argument("In scenario group '" + groupId + "', scenario '"
                          + std::to_string(scenario) + "' does not exist.")
{
}

} // namespace Antares::Solver::Modeler::DataSeries
