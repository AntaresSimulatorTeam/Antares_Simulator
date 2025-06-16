#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "antares/optimisation/linear-problem-api/IScenario.h"

namespace Antares::Optimization
{
class ScenarioGroupRepository
{
public:
    void addScenario(const std::string& groupId,
                     std::unique_ptr<Optimisation::LinearProblemApi::IScenario> scenario);

    [[nodiscard]] Optimisation::LinearProblemApi::IScenario& scenario(
      const std::string& groupId) const;

private:
    std::map<std::string, std::unique_ptr<Optimisation::LinearProblemApi::IScenario>>
      scenarioGroups_;

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
};
} // namespace Antares::Optimization
