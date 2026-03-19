// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimisation
{

OptimEntityContainer::OptimEntityContainer(LinearProblemApi::ILinearProblem& linearProblem):
    linearProblem_(linearProblem)
{
}

unsigned OptimEntityContainer::getVariableStartColumn(const Component& component,
                                                      unsigned index) const
{
    const auto& optimComponent = optimComponents_.at(component.Id());
    return variableStartColumn_.at(optimComponent.modelVariableGlobalIndices.at(index));
}

unsigned OptimEntityContainer::getConstraintStartLine(const Component& component,
                                                      unsigned index) const
{
    const auto& optimComponent = optimComponents_.at(component.Id());
    return optimComponent.modelConstraintStartLines.at(index);
}

VariabilityType OptimEntityContainer::getConstraintVariability(const Component& component,
                                                               unsigned index) const
{
    const auto& optimComponent = optimComponents_.at(component.Id());
    return optimComponent.modelConstraintsVariability.at(index);
}

void OptimEntityContainer::addStartColumn()
{
    variableStartColumn_.push_back(linearProblem_.variableCount());
}

std::span<const std::unique_ptr<IMipVariable>> OptimEntityContainer::getComponentVariable(
  const Component& component,
  unsigned index,
  std::size_t nbTimeSteps) const
{
    const auto& variables = linearProblem_.getVariables();
    unsigned startColumn = getVariableStartColumn(component, index);
    return {variables.data() + startColumn, nbTimeSteps};
}

std::span<const std::unique_ptr<IMipConstraint>> OptimEntityContainer::componentConstraints(
  const Component& component,
  unsigned index,
  std::size_t nbTimeSteps) const
{
    const auto& constraints = linearProblem_.getConstraints();
    unsigned startLine = getConstraintStartLine(component, index);
    return {constraints.data() + startLine, nbTimeSteps};
}

void OptimEntityContainer::addFromSystemComponents(const std::vector<Component>& components,
                                                   Solver::Config::Location targetLocation)
{
    optimComponents_.clear();
    unsigned variableGlobalIndex = 0;
    for (const auto& component: components)
    {
        const auto* model = component.getModel();
        const auto& variables = model->Variables();
        std::vector<unsigned> modelVariableGlobalIndices;

        modelVariableGlobalIndices.reserve(variables.size());
        for (const auto& variable: variables)
        {
            if (AreLocationsCompatibleForFillers(variable.location(), targetLocation))
            {
                modelVariableGlobalIndices.push_back(variableGlobalIndex);
                ++variableGlobalIndex;
            }
            else
            {
                // We add dummy data to avoid "holes" in modelVariableGlobalIndices
                // but these are not to be used
                modelVariableGlobalIndices.push_back(-1);
            }
        }

        optimComponents_.emplace(component.Id(),
                                 OptimComponent{.modelVariableGlobalIndices =
                                                  std::move(modelVariableGlobalIndices),
                                                .modelConstraintStartLines = {},
                                                .modelConstraintsVariability = {},
                                                .modelConstraintCounts = {}});
    }
}

void OptimEntityContainer::registerConstraint(const Component& component,
                                              const VariabilityType& variability,
                                              unsigned count)
{
    auto& optimComponent = optimComponents_.at(component.Id());
    optimComponent.modelConstraintStartLines.push_back(
      static_cast<unsigned>(linearProblem_.constraintCount()));
    optimComponent.modelConstraintsVariability.push_back(variability);
    optimComponent.modelConstraintCounts.push_back(count);
}

} // namespace Antares::Optimisation
