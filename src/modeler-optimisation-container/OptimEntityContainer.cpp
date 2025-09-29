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
    std::unordered_map<std::string, unsigned int> variableIndexMap;
    variableIndexMap.reserve(variables.size());

    unsigned int variableLocalIndex = 0;
    for (const auto& variable: variables)
    {
        variableIndexMap[variable.Id()] = variableGlobalIndex_; // used in
        // ReadlinearExpressionVisitor
        ++variableGlobalIndex_;
        ++variableLocalIndex;
    }
    optimComponents_.push_back(
      {.index = component.Index(),
       .variableIndexMap = variableIndexMap,
       .evaluationContext = Optimisation::EvaluationContext(&component,
                                                            data_,
                                                            &scenarioGroupRepository_->scenario(
                                                              component.getScenarioGroupId()))});
}
} // namespace Antares::Optimisation
