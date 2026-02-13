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

    unsigned getVariableStartColumn(const ModelerStudy::SystemModel::Component& component,
                                    unsigned index) const;

    const EvaluationContext& getEvaluationContext(
      const ModelerStudy::SystemModel::Component& component) const;

    unsigned getConstraintStartLine(const ModelerStudy::SystemModel::Component& component,
                                    unsigned index) const;
    VariabilityType getConstraintVariability(const ModelerStudy::SystemModel::Component& component,
                                             unsigned index) const;

    LinearProblemApi::ILinearProblem& Problem();

    void addStartColumn();

    std::span<const std::unique_ptr<LinearProblemApi::IMipVariable>> getComponentVariable(
      const ModelerStudy::SystemModel::Component& component,
      unsigned int index,
      std::size_t nbTimeSteps) const;

    std::pair<std::span<const std::unique_ptr<LinearProblemApi::IMipConstraint>>, VariabilityType>
    getComponentConstraint(const ModelerStudy::SystemModel::Component& component,
                           unsigned int index,
                           std::size_t nbTimeSteps) const;

    OptimComponent& getOptimComponent(size_t index);

    void addFromSystemComponents(
      const std::vector<ModelerStudy::SystemModel::Component>& component,
      Solver::Config::Location targetLocation = Solver::Config::Location::SUBPROBLEMS);

    void registerConstraint(const ModelerStudy::SystemModel::Component& component,
                            const VariabilityType& variability);

    unsigned constraintGLobalIndex() const;

private:
    void addStartLine();

    std::vector<unsigned int> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    std::vector<unsigned int> constraintStartLine_;
    LinearProblemApi::ILinearProblem& linearProblem_;
    const LinearProblemApi::ILinearProblemData* data_;
    const ScenarioGroupRepository* scenarioGroupRepository_;
};
} // namespace Antares::Optimisation
