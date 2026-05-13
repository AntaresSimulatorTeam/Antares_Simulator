// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/MipDetection.h"

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>

namespace Antares::Optimization
{

static bool hasModelerIntegerVariables(const Solver::ModelerData* modelerData,
                                       bool lookingAtSubproblems)
{
    if (!modelerData || !modelerData->system)
    {
        return false;
    }

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            if (lookingAtSubproblems && !isInSubProblem(variable.location()))
            {
                continue;
            }
            if (!lookingAtSubproblems && !isInMasterProblem(variable.location()))
            {
                continue;
            }
            if (variable.Type() != ModelerStudy::SystemModel::ValueType::FLOAT)
            {
                return true;
            }
        }
    }
    return false;
}

bool hasSubproblemIntegerVariables(const Solver::ModelerData* modelerData)
{
    return hasModelerIntegerVariables(modelerData, true);
}

bool hasMasterIntegerVariables(const Solver::ModelerData* modelerData)
{
    return hasModelerIntegerVariables(modelerData, false);
}

} // namespace Antares::Optimization
