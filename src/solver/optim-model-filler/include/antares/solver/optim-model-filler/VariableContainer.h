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
    const ModelerStudy::SystemModel::Component* component;
    std::vector<unsigned int> modelVariablesGlobalIndices = {};
    std::unordered_map<std::string, unsigned int> variableIndexMap;
};

struct OptimModel
{
    unsigned int index = 0;
    const ModelerStudy::SystemModel::Model* model;
    std::vector<OptimComponent> optimComponents{};
};

class VariableContainer
{
public:
    VariableContainer() = default;

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

    [[nodiscard]] const OptimComponent& getOptimComponent(size_t index, unsigned modelIndex) const
    {
        const auto& optimModel = optimModels_.at(modelIndex);
        auto it = std::ranges::find_if(optimModel.optimComponents,
                                       [&index](const OptimComponent& optimComponent)
                                       { return optimComponent.index == index; });
        if (it != optimModel.optimComponents.end())
        {
            return *it;
        }
        else
        {
            throw std::invalid_argument("OptimComponent not found");
        }
    }


    void addFromSystemComponent(const Antares::ModelerStudy::SystemModel::Component& component);
    void reserveOptimModels(const std::vector<const ModelerStudy::SystemModel::Model*>& models);

    [[nodiscard]] const std::vector<OptimModel>& getOptimModels() const
    {
        return optimModels_;
    }

private:
    std::vector<Antares::Optimisation::LinearProblemApi::IMipVariable*> variables_;
    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    std::vector<OptimModel> optimModels_;
    unsigned int variableGlobalIndex_ = 0;
};
} // namespace Antares::Optimisation
