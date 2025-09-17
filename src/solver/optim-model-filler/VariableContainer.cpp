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
#include "antares/solver/optim-model-filler/VariableContainer.h"

namespace Antares::Optimisation
{

void VariableContainer::addFromSystemComponent(
  const Antares::ModelerStudy::SystemModel::Component& component)
{
    const auto* model = component.getModel();
    unsigned int modelIndex = model->Index();
    optimModels_[modelIndex].optimComponents.push_back(
      {.index = component.Index(),
       .component = &component,
                                                        .modelVariablesGlobalIndices = {},
                                                        .variableIndexMap = {}});
}

void VariableContainer::reserveOptimModels(
  const std::vector<const ModelerStudy::SystemModel::Model*>& models)
{
    optimModels_.assign(models.size(), {});
    for (const auto& model: models)
    {
        auto& [index, targetModel, _] = optimModels_[model->Index()];
        index = model->Index();
        targetModel = model;
    }
}
} // namespace Antares::Optimisation
