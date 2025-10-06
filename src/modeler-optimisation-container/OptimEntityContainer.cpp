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

#include "antares/modeler-optimisation-container/OptimEntityContainer.h"

#include "antares/optimisation/linear-problem-api/ILinearProblemData.h"

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

void OptimEntityContainer::addFromSystemComponent(
  const Antares::ModelerStudy::SystemModel::Component& component)
{
    const auto* model = component.getModel();
    const auto& variables = model->Variables();
    std::vector<unsigned int> modelVariableGlobalIndices;
    modelVariableGlobalIndices.resize(variables.size());

    for (auto variableLocalIndex = 0; variableLocalIndex < variables.size(); ++variableLocalIndex)
    {
        modelVariableGlobalIndices[variableLocalIndex] = variableGlobalIndex_; // used in
        // ReadlinearExpressionVisitor
        ++variableGlobalIndex_;
    }
    optimComponents_.push_back(
      {.index = component.Index(),
       .modelVariableGlobalIndices = modelVariableGlobalIndices,
       .evaluationContext = Optimisation::EvaluationContext(&component,
                                                            data_,
                                                            &scenarioGroupRepository_->scenario(
                                                              component.getScenarioGroupId()))});
}

void OptimEntityContainer::registerConstraint(const ModelerStudy::SystemModel::Component& component,
                                              const TimeIndex& timeIndex)
{
    const auto gLobalIndex = ConstraintGLobalIndex();
    auto& optimComponent = getOptimComponent(component.Index());
    optimComponent.modelConstraintsGlobalIndices.push_back(gLobalIndex);
    optimComponent.modelConstraintsTimeIndex.push_back(timeIndex);
    addStartLine();
}
} // namespace Antares::Optimisation
