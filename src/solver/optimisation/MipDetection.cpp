// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/MipDetection.h"

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/optimConfig.h>

namespace Antares::Optimization
{

bool hasIntegerVariables(const Solver::ModelerData* modelerData,
                         Antares::Solver::Config::Location location)
{
    if (!modelerData || !modelerData->system)
    {
        return false;
    }

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            if (location == Antares::Solver::Config::Location::SUBPROBLEMS
                && !isInSubProblem(variable.location()))
            {
                continue;
            }
            if (location == Antares::Solver::Config::Location::MASTER
                && !isInMasterProblem(variable.location()))
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

} // namespace Antares::Optimization
