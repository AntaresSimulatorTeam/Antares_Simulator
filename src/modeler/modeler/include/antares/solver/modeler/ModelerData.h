// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include <antares/modeler-optimisation-container/scenarioGroupRepo.h>
#include <antares/optimisation/linear-problem-data-impl/linearProblemData.h>
#include <antares/study/system-model/library.h>
#include <antares/study/system-model/system.h>

namespace Antares::Solver
{

enum class ResolutionMode : unsigned
{
    BENDERS_DECOMPOSITION = 0,
    SEQUENTIAL_SUBPROBLEMS = 1
};

struct ModelerData
{
    std::vector<ModelerStudy::SystemModel::Library> libraries;
    std::unique_ptr<ModelerStudy::SystemModel::System> system;
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> dataSeries;
    Optimisation::ScenarioGroupRepository scenarioGroupRepository;
    ResolutionMode resolutionMode = ResolutionMode::SEQUENTIAL_SUBPROBLEMS;
};

} // namespace Antares::Solver
