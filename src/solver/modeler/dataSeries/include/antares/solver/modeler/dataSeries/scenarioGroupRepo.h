
#pragma once
#include <map>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
class ScenarioGroupRepository
{
public:
    unsigned int getDataRank(std::string groupId, unsigned int scenario);

private:
    std::map<std::string, std::map<unsigned int, unsigned int>> scenarioGroups_;
};
} // namespace Antares::Solver::Modeler::DataSeries
