// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "antares/optimisation/linear-problem-api/IScenario.h"

namespace Antares::LinearProblem
{
class ScenarioGroupRepository final
{
public:
    void addScenario(const std::string& groupId, std::unique_ptr<Api::IScenario>&& scenario);

    [[nodiscard]] const Api::IScenario& scenario(const std::string& groupId) const;

    [[nodiscard]] bool contains(const std::string& groupId) const;

private:
    std::map<std::string, std::unique_ptr<Api::IScenario>> scenarioGroups_;

public:
    class AlreadyExists final: public std::invalid_argument
    {
    public:
        explicit AlreadyExists(const std::string& groupId);
    };

    class DoesNotExist final: public std::invalid_argument
    {
    public:
        explicit DoesNotExist(const std::string& groupId);
    };
};
} // namespace Antares::LinearProblem
