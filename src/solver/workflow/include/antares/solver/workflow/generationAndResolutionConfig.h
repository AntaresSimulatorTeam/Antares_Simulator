// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/solver/modeler/ModelerData.h>
#include <antares/study/study.h>

namespace Antares::Solver::Workflow
{

enum class SolverType : unsigned
{
    LP,
    MILP
};

struct GenerationAndResolutionConfig
{
    SolverType master = SolverType::LP;
    SolverType subproblems = SolverType::LP;
    /// Legacy thermal clusters: run the legacy heuristic (fast or accurate) between two
    /// optimisations
    bool useThermalHeuristic = true;
    /// GEMS components with integer strategy "heuristic": run their heuristics between two
    /// optimisations
    bool useGemsThermalHeuristics = false;
    ResolutionMode resolutionMode = ResolutionMode::SEQUENTIAL_SUBPROBLEMS;
};

GenerationAndResolutionConfig getWorkflow(const Data::Study& study);

} // namespace Antares::Solver::Workflow
