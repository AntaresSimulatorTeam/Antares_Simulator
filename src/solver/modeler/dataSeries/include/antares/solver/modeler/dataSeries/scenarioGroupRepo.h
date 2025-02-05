
#pragma once
#include <map>
#include <stdexcept>
#include <string>

namespace Antares::Solver::Modeler::DataSeries
{
class ScenarioGroupRepository
{
public:
    void addPairScenarioRankToGroup(const std::string& groupId,
                                    const std::pair<unsigned, unsigned> scenarioToRank);

    unsigned getDataRank(const std::string& groupId, const unsigned scenario);

private:
    std::map<std::string, std::map<unsigned, unsigned>> scenarioGroups_;

public:
    class AlreadyExists: public std::invalid_argument
    {
    public:
        explicit AlreadyExists(const std::string& groupId);
    };

    class DoesNotExist: public std::invalid_argument
    {
    public:
        explicit DoesNotExist(const std::string& groupId);
    };

    class ScenarioNotExist: public std::invalid_argument
    {
    public:
        explicit ScenarioNotExist(const std::string& groupId, unsigned scenario);
    };
};
} // namespace Antares::Solver::Modeler::DataSeries
