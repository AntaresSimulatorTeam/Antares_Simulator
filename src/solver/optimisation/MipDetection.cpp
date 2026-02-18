// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/MipDetection.h"

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>

namespace Antares::Optimization
{

bool hasModelerIntegerVariables(const Solver::ModelerData* modelerData)
{
    if (!modelerData || !modelerData->system)
    {
        return false;
    }

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            if (variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT)
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace Antares::Optimization
