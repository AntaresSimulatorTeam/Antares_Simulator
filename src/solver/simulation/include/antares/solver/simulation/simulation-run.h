// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/solver/simulation/solver.h"

namespace Antares::Solver
{

Benchmarking::OptimizationInfo simulationRun(Antares::Data::Study& study,
                                             const Settings& settings,
                                             Benchmarking::DurationCollector& durationCollector,
                                             IResultWriter& resultWriter,
                                             Simulation::ISimulationObserver& simulationObserver);

} // namespace Antares::Solver
