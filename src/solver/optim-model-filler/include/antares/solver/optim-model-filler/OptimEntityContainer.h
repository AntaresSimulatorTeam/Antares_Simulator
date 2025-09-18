/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once
#include <vector>

#include <antares/optimisation/linear-problem-api/mipVariable.h>
#include <antares/study/system-model/component.h>

namespace Antares::Optimisation
{
struct OptimComponent
{
    unsigned int index = 0;
    std::vector<unsigned int> modelVariablesGlobalIndices = {};
    std::unordered_map<std::string, unsigned int> variableIndexMap;
};

class OptimEntityContainer
{
public:
    OptimEntityContainer() = default;

    [[nodiscard]] const std::vector<unsigned int>& getVariableStartColumn() const
    {
        return variableStartColumn_;
    }

    void addStartColumn()
    {
        variableStartColumn_.push_back(variables_.size());
    }

    [[nodiscard]] const std::vector<LinearProblemApi::IMipVariable*>& getVariables() const
    {
        return variables_;
    }

    [[nodiscard]] size_t variablesSize() const
    {
        return variables_.size();
    }

    void addVariable(LinearProblemApi::IMipVariable* variable)
    {
        variables_.push_back(variable);
    }

    [[nodiscard]] const OptimComponent& getOptimComponent(size_t index) const
    {
        return optimComponents_.at(index);
    }

    [[nodiscard]] const std::vector<OptimComponent>& getOptimComponents() const
    {
        return optimComponents_;
    }

    void addFromSystemComponent(const Antares::ModelerStudy::SystemModel::Component& component);

private:
    std::vector<Antares::Optimisation::LinearProblemApi::IMipVariable*> variables_;
    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    unsigned int variableGlobalIndex_ = 0;
};
} // namespace Antares::Optimisation
