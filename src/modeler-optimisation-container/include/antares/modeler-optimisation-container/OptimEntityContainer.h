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

     // gp : to be removed : used to avoid passing the linear problem.
    LinearProblemApi::ILinearProblem& Problem(); 

    void addStartColumn();

    std::span<const std::unique_ptr<LinearProblemApi::IMipVariable>> getComponentVariable(
      const ModelerStudy::SystemModel::Component& component,
      unsigned index,
      std::size_t nbTimeSteps) const;

    std::span<const std::unique_ptr<LinearProblemApi::IMipConstraint>> componentConstraints(
      const ModelerStudy::SystemModel::Component& component,
      unsigned index,
      std::size_t nbTimeSteps) const;

    void addFromSystemComponents(
      const std::vector<ModelerStudy::SystemModel::Component>& component,
      Solver::Config::Location targetLocation = Solver::Config::Location::SUBPROBLEMS);

    void registerConstraint(const ModelerStudy::SystemModel::Component& component,
                            const VariabilityType& variability);

private:
    void addStartLine();

    std::vector<unsigned> variableStartColumn_;
    std::vector<OptimComponent> optimComponents_;
    std::vector<unsigned> constraintStartLine_;
    LinearProblemApi::ILinearProblem& linearProblem_;
};
} // namespace Antares::Optimisation
