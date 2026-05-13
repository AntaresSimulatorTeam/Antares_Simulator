// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/workflow/generationAndResolutionConfig.h"

#include <antares/solver/optimisation/MipDetection.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/optimConfig.h>

namespace Antares::Solver::Workflow
{

namespace
{

bool hasMasterIntegerVariables(const Solver::ModelerData* modelerData)
{
    if (!modelerData || !modelerData->system)
    {
        return false;
    }

    for (const auto& component: modelerData->system->Components())
    {
        for (const auto& variable: component.getModel()->Variables())
        {
            auto loc = variable.location();
            if (loc != Solver::Config::Location::MASTER
                && loc != Solver::Config::Location::MASTER_AND_SUBPROBLEMS)
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

} // anonymous namespace

GenerationAndResolutionConfig generate(const Data::Study& study)
{
    auto* modelerData = study.getModelerData();
    const auto ucMode = study.parameters.unitCommitment.ucMode;

    auto resMode = modelerData->resolutionMode;
    bool heuristic = (ucMode == Data::ucHeuristicFast || ucMode == Data::ucHeuristicAccurate);
    bool subpMilp = (ucMode == Data::ucMILP
                     || Antares::Optimization::hasModelerIntegerVariables(modelerData));

    GenerationAndResolutionConfig config;
    config.resolution_mode = resMode;
    config.use_heuristic = heuristic;
    config.subproblems = subpMilp ? SolverType::MILP : SolverType::LP;
    config.master = (resMode == ResolutionMode::BENDERS_DECOMPOSITION)
                      ? (hasMasterIntegerVariables(modelerData) ? SolverType::MILP : SolverType::LP)
                      : SolverType::MILP;
    return config;
}

} // namespace Antares::Solver::Workflow
