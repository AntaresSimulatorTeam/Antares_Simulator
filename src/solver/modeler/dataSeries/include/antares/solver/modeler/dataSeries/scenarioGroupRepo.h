
#pragma once
#include <map>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
class ScenarioGroupRepository
{
public:
    void addPairScenarioRankToGroup(std::string groupId,
                                    std::pair<unsigned, unsigned> scenarioToRank);

    unsigned getDataRank(std::string groupId, unsigned scenario);

private:
    std::map<std::string, std::map<unsigned, unsigned>> scenarioGroups_;
};
} // namespace Antares::Solver::Modeler::DataSeries
