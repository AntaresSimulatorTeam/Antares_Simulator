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
#include <span>
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
    std::vector<unsigned int> modelVariableGlobalIndices;
    std::vector<unsigned int> modelConstraintsGlobalIndices;
    std::vector<TimeIndex> modelConstraintsTimeIndex;
    EvaluationContext evaluationContext;
};

class OptimEntityContainer
{
public:
    OptimEntityContainer(LinearProblemApi::ILinearProblem& linearProblem,
                         const LinearProblemApi::ILinearProblemData* data,
                         const ScenarioGroupRepository* scenarioGroupRepository);

    [[nodiscard]] unsigned int getVariableStartColumn(
      const Antares::ModelerStudy::SystemModel::Component& component,
      unsigned int index) const
    {
        const auto& optimComponent = optimComponents_.at(component.Index());
        return variableStartColumn_.at(optimComponent.modelVariableGlobalIndices.at(index));
    }

    [[nodiscard]] const EvaluationContext& getEvaluationContext(
      const Antares::ModelerStudy::SystemModel::Component& component) const
    {
        const auto& optimComponent = optimComponents_.at(component.Index());
        return optimComponent.evaluationContext;
    }

    [[nodiscard]] std::pair<unsigned int, TimeIndex> getConstraintData(
      const Antares::ModelerStudy::SystemModel::Component& component,
      unsigned int index) const
    {
        const auto& optimComponent = optimComponents_.at(component.Index());
        return {constraintStartLine_.at(optimComponent.modelConstraintsGlobalIndices.at(index)),
                optimComponent.modelConstraintsTimeIndex.at(index)};
    }

    LinearProblemApi::ILinearProblem& Problem()
    {
        return linearProblem_;
    }

    void addStartColumn()
    {
        variableStartColumn_.push_back(linearProblem_.variableCount());
    }

    [[nodiscard]] const std::vector<std::unique_ptr<LinearProblemApi::IMipVariable>>& getVariables()
      const
    {
        return linearProblem_.getVariables();
    }

    [[nodiscard]] std::span<const std::unique_ptr<LinearProblemApi::IMipVariable>>
    getComponentVariable(const Antares::ModelerStudy::SystemModel::Component& component,
                         unsigned int index,
                         std::size_t nbTimeSteps) const
    {
        const auto& variables = linearProblem_.getVariables();
        unsigned int startColumn = getVariableStartColumn(component, index);
        return {variables.data() + startColumn, nbTimeSteps};
    }

    [[nodiscard]] std::pair<std::span<const std::unique_ptr<LinearProblemApi::IMipConstraint>>,
                            TimeIndex>
    getComponentConstraint(const Antares::ModelerStudy::SystemModel::Component& component,
                           unsigned int index,
                           std::size_t nbTimeSteps) const
    {
        const auto& constraints = linearProblem_.getConstraints();
        const auto [startLine, timeIndex] = getConstraintData(component, index);
        return {{constraints.data() + startLine, nbTimeSteps}, timeIndex};
    }

    [[nodiscard]] const std::vector<std::unique_ptr<LinearProblemApi::IMipConstraint>>&
    getConstraints() const
    {
        return linearProblem_.getConstraints();
    }

    [[nodiscard]] const OptimComponent& getOptimComponent(size_t index) const
    {
        return optimComponents_.at(index);
    }

    [[nodiscard]] OptimComponent& getOptimComponent(size_t index)
    {
        return optimComponents_.at(index);
    }

    void addFromSystemComponents(
      const std::vector<Antares::ModelerStudy::SystemModel::Component>& component);
    void registerConstraint(const ModelerStudy::SystemModel::Component& component,
                            const TimeIndex& timeIndex);

    unsigned int ConstraintGLobalIndex() const
    {
        return static_cast<unsigned int>(constraintStartLine_.size());
    }

private:
    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    unsigned int variableGlobalIndex_ = 0;
    std::vector<unsigned int> constraintStartLine_;
    LinearProblemApi::ILinearProblem& linearProblem_;
    const LinearProblemApi::ILinearProblemData* data_;
    const ScenarioGroupRepository* scenarioGroupRepository_;

    void addStartLine()
    {
        constraintStartLine_.push_back(linearProblem_.constraintCount());
    }
};
} // namespace Antares::Optimisation
