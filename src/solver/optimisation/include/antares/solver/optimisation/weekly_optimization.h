// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

class OptimisationsSimulationTable;

namespace Antares::Solver::Optimization
{

class WeeklyOptimization final
{
public:
    WeeklyOptimization(const OptimizationOptions& options,
                       PROBLEME_HEBDO* problemeHebdo,
                       IResultWriter& writer,
                       Simulation::ISimulationObserver& simulationObserver,
                       OptimisationsSimulationTable* simulationTables);
    ~WeeklyOptimization() = default;
    void solve();

private:
    Antares::Solver::Optimization::OptimizationOptions options_;
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    IResultWriter& writer_;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;
    OptimisationsSimulationTable* simulationTables_ = nullptr;
};
} // namespace Antares::Solver::Optimization
