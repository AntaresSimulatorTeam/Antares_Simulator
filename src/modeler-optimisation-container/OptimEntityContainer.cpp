// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

using namespace Antares::ModelerStudy::SystemModel;

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

void OptimEntityContainer::addFromSystemComponents(const std::vector<Component>& components,
                                                   Modeler::Config::Location targetLocation)
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
} // namespace Antares::Optimisation
