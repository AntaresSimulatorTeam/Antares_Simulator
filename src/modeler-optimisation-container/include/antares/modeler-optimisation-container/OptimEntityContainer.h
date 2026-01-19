// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once
#include <span>
#include <vector>

#include <antares/optimisation/linear-problem-api/mipConstraint.h>
#include <antares/optimisation/linear-problem-api/mipVariable.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/variabilityType.h>
#include "antares/optimisation/linear-problem-api/linearProblem.h"

#include "EvaluationContext.h"
#include "scenarioGroupRepo.h"

namespace Antares::Optimisation
{
struct OptimComponent
{
    std::vector<unsigned> modelVariableGlobalIndices;
    std::vector<unsigned> modelConstraintsGlobalIndices;
    std::vector<VariabilityType> modelConstraintsVariability;
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

    [[nodiscard]] std::pair<unsigned int, VariabilityType> getConstraintData(
      const Antares::ModelerStudy::SystemModel::Component& component,
      unsigned int index) const
    {
        const auto& optimComponent = optimComponents_.at(component.Index());
        return {constraintStartLine_.at(optimComponent.modelConstraintsGlobalIndices.at(index)),
                optimComponent.modelConstraintsVariability.at(index)};
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
                            VariabilityType>
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

    [[nodiscard]] OptimComponent& getOptimComponent(size_t index)
    {
        return optimComponents_.at(index);
    }

    void addFromSystemComponents(
      const std::vector<Antares::ModelerStudy::SystemModel::Component>& component,
      Modeler::Config::Location targetLocation = Modeler::Config::Location::SUBPROBLEMS);
    void registerConstraint(const ModelerStudy::SystemModel::Component& component,
                            const VariabilityType& variability);

    unsigned constraintGLobalIndex() const
    {
        return static_cast<unsigned int>(constraintStartLine_.size());
    }

private:
    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
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
