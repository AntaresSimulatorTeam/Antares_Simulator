// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimisation
{

OptimEntityContainer::OptimEntityContainer(LinearProblemApi::ILinearProblem& linearProblem,
                                           const LinearProblemApi::ILinearProblemData* data,
                                           const ScenarioGroupRepository* scenarioGroupRepository):
    linearProblem_(linearProblem),
    data_(data),
    scenarioGroupRepository_(scenarioGroupRepository)
{
}

unsigned OptimEntityContainer::getVariableStartColumn(const Component& component,
                                                      unsigned index) const
{
    const auto& optimComponent = optimComponents_.at(component.Index());
    return variableStartColumn_.at(optimComponent.modelVariableGlobalIndices.at(index));
}

const EvaluationContext& OptimEntityContainer::getEvaluationContext(
  const Component& component) const
{
    const auto& optimComponent = optimComponents_.at(component.Index());
    return optimComponent.evaluationContext;
}

std::pair<unsigned, VariabilityType> OptimEntityContainer::getConstraintData(
  const Component& component,
  unsigned index) const
{
    const auto& optimComponent = optimComponents_.at(component.Index());
    return {constraintStartLine_.at(optimComponent.modelConstraintsGlobalIndices.at(index)),
            optimComponent.modelConstraintsVariability.at(index)};
}

ILinearProblem& OptimEntityContainer::Problem()
{
    return linearProblem_;
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
    unsigned int startColumn = getVariableStartColumn(component, index);
    return {variables.data() + startColumn, nbTimeSteps};
}

std::pair<std::span<const std::unique_ptr<IMipConstraint>>, VariabilityType>
OptimEntityContainer::getComponentConstraint(const Component& component,
                                             unsigned int index,
                                             std::size_t nbTimeSteps) const
{
    const auto& constraints = linearProblem_.getConstraints();
    const auto [startLine, timeIndex] = getConstraintData(component, index);
    return {{constraints.data() + startLine, nbTimeSteps}, timeIndex};
}

OptimComponent& OptimEntityContainer::getOptimComponent(size_t index)
{
    return optimComponents_.at(index);
}

unsigned OptimEntityContainer::constraintGLobalIndex() const
{
    return static_cast<unsigned int>(constraintStartLine_.size());
}

void OptimEntityContainer::addFromSystemComponents(const std::vector<Component>& components,
                                                   Solver::Config::Location targetLocation)
{
    optimComponents_.clear();
    optimComponents_.reserve(components.size());
    unsigned variableGlobalIndex = 0;
    for (const auto& component: components)
    {
        const auto* model = component.getModel();
        const auto& variables = model->Variables();
        std::vector<unsigned int> modelVariableGlobalIndices;

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
        optimComponents_.push_back(
          {.modelVariableGlobalIndices = modelVariableGlobalIndices,
           .evaluationContext = Optimisation::EvaluationContext(
             &component,
             data_,
             &scenarioGroupRepository_->scenario(component.getScenarioGroupId()))});
    }
}

void OptimEntityContainer::registerConstraint(const Component& component,
                                              const VariabilityType& variability)
{
    unsigned gLobalIndex = constraintGLobalIndex();
    auto& optimComponent = getOptimComponent(component.Index());
    optimComponent.modelConstraintsGlobalIndices.push_back(gLobalIndex);
    optimComponent.modelConstraintsVariability.push_back(variability);
    addStartLine();
}

void OptimEntityContainer::addStartLine()
{
    constraintStartLine_.push_back(linearProblem_.constraintCount());
}

} // namespace Antares::Optimisation
