
#pragma once
#include <map>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
class ScenarioGroupRepository
{
public:
    void addPairScenarioRankToGroup(const std::string groupId,
                                    const std::pair<unsigned, unsigned> scenarioToRank);

    unsigned getDataRank(const std::string& groupId, const unsigned scenario);

private:
    std::map<std::string, std::map<unsigned, unsigned>> scenarioGroups_;
};
} // namespace Antares::Solver::Modeler::DataSeries
