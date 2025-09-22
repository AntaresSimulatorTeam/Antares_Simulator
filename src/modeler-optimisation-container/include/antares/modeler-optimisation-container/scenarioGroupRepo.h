#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "antares/optimisation/linear-problem-api/IScenario.h"

namespace Antares::Optimisation
{
class ScenarioGroupRepository
{
public:
    void addScenario(const std::string& groupId,
                     std::unique_ptr<LinearProblemApi::IScenario>&& scenario);

    [[nodiscard]] const LinearProblemApi::IScenario& scenario(const std::string& groupId) const;

private:
    std::map<std::string, std::unique_ptr<LinearProblemApi::IScenario>> scenarioGroups_;

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
} // namespace Antares::Optimisation
