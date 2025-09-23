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

#include <antares/optimisation/linear-problem-api/mipConstraint.h>
#include <antares/optimisation/linear-problem-api/mipVariable.h>
#include <antares/study/system-model/component.h>

#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "EvaluationContext.h"
#include "TimeIndex.h"
#include "scenarioGroupRepo.h"

namespace Antares::Optimisation
{
struct OptimComponent
{
    unsigned int index = 0;
    std::vector<unsigned int> modelVariablesGlobalIndices = {};
    std::unordered_map<std::string, unsigned int> variableIndexMap;
    std::vector<unsigned int> modelConstraintsGlobalIndices = {};
    std::vector<TimeIndex> modelConstraintsTimeIndex = {};
    EvaluationContext evaluationContext;
};

class OptimEntityContainer
{
public:
    OptimEntityContainer(LinearProblemApi::ILinearProblem& linearProblem,
                         const LinearProblemApi::ILinearProblemData* data,
                         const ScenarioGroupRepository* scenarioGroupRepository);

    [[nodiscard]] const std::vector<unsigned int>& getVariableStartColumn() const
    {
        return variableStartColumn_;
    }

      [[nodiscard]] unsigned int getVariableStartColumn(size_t compoIndex,
                                                      const std::string& varName) const
    {
        const auto& optimComponent = optimComponents_[compoIndex];
        return variableStartColumn_[optimComponent.variableIndexMap.at(varName)];
    }

    [[nodiscard]] const std::vector<unsigned int>& getConstraintStartLine() const
    {
        return constraintStartLine_;
    }

    LinearProblemApi::ILinearProblem& Problem()
    {
        return linearProblem_;
    }

    void addStartColumn()
    {
        variableStartColumn_.push_back(variables_.size());
    }

    void addStartLine()
    {
        constraintStartLine_.push_back(constraints_.size());
    }

    [[nodiscard]] const std::vector<LinearProblemApi::IMipVariable*>& getVariables() const
    {
        return variables_;
    }

    [[nodiscard]] const std::vector<LinearProblemApi::IMipConstraint*>& getConstraints() const
    {
        return constraints_;
    }

    [[nodiscard]] size_t variablesSize() const
    {
        return variables_.size();
    }

  [[nodiscard]] size_t constraintsSize() const
    {
        return constraints_.size();
    }

    void registerVariable(LinearProblemApi::IMipVariable* variable)
    {
        variables_.push_back(variable);
    }

      void registerConstraint(LinearProblemApi::IMipConstraint* constraint)
    {
        constraints_.push_back(constraint);
    }

    [[nodiscard]] const OptimComponent& getOptimComponent(size_t index) const
    {
        return optimComponents_.at(index);
    }
    [[nodiscard]]  OptimComponent& getOptimComponent(size_t index) 
    {
        return optimComponents_.at(index);
    }

    [[nodiscard]] const std::vector<OptimComponent>& getOptimComponents() const
    {
        return optimComponents_;
    }

    void addFromSystemComponent(const Antares::ModelerStudy::SystemModel::Component& component);

    
    // unsigned int VariableGLobalIndex() const
    // {
    //     return variableGlobalIndex_;
    // }

    unsigned int ConstraintGLobalIndex() const
    {
        return constraintGlobalIndex_;
    }

    // void IncrementVariableGLobalIndex()
    // {
    //     ++variableGlobalIndex_;
    // }

    void IncrementConstraintGLobalIndex()
    {
        ++constraintGlobalIndex_;
    }

private:
    std::vector<LinearProblemApi::IMipVariable*> variables_;
    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    unsigned int variableGlobalIndex_ = 0;
    //---
    std::vector<LinearProblemApi::IMipConstraint*> constraints_;
    std::vector<unsigned int> constraintStartLine_;
    unsigned int constraintGlobalIndex_ = 0;
    LinearProblemApi::ILinearProblem& linearProblem_;
    const LinearProblemApi::ILinearProblemData* data_;
    const ScenarioGroupRepository* scenarioGroupRepository_;
};
} // namespace Antares::Optimisation
