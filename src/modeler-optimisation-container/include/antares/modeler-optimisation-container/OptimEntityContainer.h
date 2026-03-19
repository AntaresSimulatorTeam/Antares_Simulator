// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <unordered_map>
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
    std::vector<unsigned> modelConstraintStartLines;
    std::vector<VariabilityType> modelConstraintsVariability;
    std::vector<unsigned> modelConstraintCounts;
};

class OptimEntityContainer
{
public:
    OptimEntityContainer(LinearProblemApi::ILinearProblem& linearProblem);

    unsigned getVariableStartColumn(const ModelerStudy::SystemModel::Component& component,
                                    unsigned index) const;

    unsigned getConstraintStartLine(const ModelerStudy::SystemModel::Component& component,
                                    unsigned index) const;
    VariabilityType getConstraintVariability(const ModelerStudy::SystemModel::Component& component,
                                             unsigned index) const;

    [[nodiscard]] std::pair<unsigned int, VariabilityType> getConstraintData(
      const Antares::ModelerStudy::SystemModel::Component& component,
      unsigned int index) const
    {
        const auto& optimComponent = optimComponents_.at(component.Id());
        return {optimComponent.modelConstraintStartLines.at(index),
                optimComponent.modelConstraintsVariability.at(index)};
    }

    [[nodiscard]] unsigned getConstraintCount(
      const Antares::ModelerStudy::SystemModel::Component& component,
      unsigned int index) const
    {
        const auto& optimComponent = optimComponents_.at(component.Id());
        return optimComponent.modelConstraintCounts.at(index);
    }

    LinearProblemApi::ILinearProblem& Problem()
    {
        return linearProblem_;
    }

    std::span<const std::unique_ptr<LinearProblemApi::IMipVariable>> getComponentVariable(
      const ModelerStudy::SystemModel::Component& component,
      unsigned index,
      std::size_t nbTimeSteps) const;

    std::span<const std::unique_ptr<LinearProblemApi::IMipConstraint>> componentConstraints(
      const ModelerStudy::SystemModel::Component& component,
      unsigned index,
      std::size_t nbTimeSteps) const;

    void addStartColumn();

    void addFromSystemComponents(
      const std::vector<ModelerStudy::SystemModel::Component>& component,
      Solver::Config::Location targetLocation = Solver::Config::Location::SUBPROBLEMS);

    void registerConstraint(const ModelerStudy::SystemModel::Component& component,
                            const VariabilityType& variability,
                            unsigned count = 1);

private:
    std::vector<unsigned int> variableStartColumn_;
    std::unordered_map<std::string, OptimComponent> optimComponents_;
    LinearProblemApi::ILinearProblem& linearProblem_;
};
} // namespace Antares::Optimisation
