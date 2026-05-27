// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/workflow/generationAndResolutionConfig.h"

#include <antares/solver/optimisation/MipDetection.h>
#include <antares/study/system-model/component.h>
#include <antares/study/system-model/model.h>
#include <antares/study/system-model/optimConfig.h>

using namespace Antares::Optimization;
using namespace Antares::Solver::Config;

namespace Antares::Solver::Workflow
{
static GenerationAndResolutionConfig buildLegacyConfig(Data::UnitCommitmentMode ucMode)
{
    GenerationAndResolutionConfig config;
    config.useThermalHeuristic = (ucMode == Data::ucHeuristicFast
                                  || ucMode == Data::ucHeuristicAccurate);
    if (config.useThermalHeuristic)
    {
        config.subproblems = SolverType::LP;
    }
    else
    {
        config.subproblems = SolverType::MILP;
    }
    return config;
}

static SolverType determineMasterSolverType(ResolutionMode resMode, const ModelerData* modelerData)
{
    if (resMode == ResolutionMode::BENDERS_DECOMPOSITION
        && !hasIntegerVariables(modelerData, Location::MASTER))
    {
        return SolverType::LP;
    }
    return SolverType::MILP;
}

static SolverType determineSubproblemSolverType(Data::UnitCommitmentMode ucMode,
                                                const ModelerData* modelerData)
{
    if (ucMode == Data::ucMILP || hasIntegerVariables(modelerData, Location::SUBPROBLEMS))
    {
        return SolverType::MILP;
    }

    return SolverType::LP;
}

static void applyModelerConfig(GenerationAndResolutionConfig& config,
                               const ModelerData* modelerData,
                               Data::UnitCommitmentMode ucMode)
{
    auto resMode = modelerData->resolutionMode;
    config.resolutionMode = resMode;
    config.master = determineMasterSolverType(resMode, modelerData);
    config.subproblems = determineSubproblemSolverType(ucMode, modelerData);
}

GenerationAndResolutionConfig getWorkflow(const Data::Study& study)
{
    // legacy behavior is default
    const auto ucMode = study.parameters.unitCommitment.ucMode;
    auto config = buildLegacyConfig(ucMode);

    // modeler allows more complex config
    if (auto* modelerData = study.getModelerData(); modelerData)
    {
        applyModelerConfig(config, modelerData, ucMode);
    }
    return config;
}

} // namespace Antares::Solver::Workflow
