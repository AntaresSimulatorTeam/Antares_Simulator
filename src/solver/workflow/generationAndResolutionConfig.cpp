// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/workflow/generationAndResolutionConfig.h"

#include <antares/solver/optimisation/MipDetection.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/optimConfig.h>

using namespace Antares::Optimization;

namespace Antares::Solver::Workflow
{

GenerationAndResolutionConfig generate(const Data::Study& study)
{
    GenerationAndResolutionConfig config;

    // legacy behavior is default
    const auto ucMode = study.parameters.unitCommitment.ucMode;
    config.heuristic = (ucMode == Data::ucHeuristicFast || ucMode == Data::ucHeuristicAccurate);
    config.subproblems = config.heuristic ? SolverType::LP : SolverType::MILP;

    // modeler allows more complex config
    if (auto* modelerData = study.getModelerData(); modelerData)
    {
        auto resMode = modelerData->resolutionMode;
        bool subpMilp = (ucMode == Data::ucMILP || hasSubproblemIntegerVariables(modelerData));

        config.resolutionMode = resMode;
        config.master = (resMode == ResolutionMode::BENDERS_DECOMPOSITION)
                          ? (hasMasterIntegerVariables(modelerData) ? SolverType::MILP
                                                                    : SolverType::LP)
                          : SolverType::MILP;
        config.subproblems = subpMilp ? SolverType::MILP : SolverType::LP;
    }
    return config;
}

} // namespace Antares::Solver::Workflow
