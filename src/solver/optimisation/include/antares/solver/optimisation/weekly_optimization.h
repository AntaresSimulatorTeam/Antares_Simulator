// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Optimization
{

class WeeklyOptimization final
{
public:
    WeeklyOptimization(const OptimizationOptions& options,
                       PROBLEME_HEBDO* problemeHebdo,
                       IResultWriter& writer,
                       Simulation::ISimulationObserver& simulationObserver,
                       bool writeSimuTable);
    ~WeeklyOptimization() = default;
    void solve();
    OptimisationsSimulationTable* simulationTables();

private:
    Antares::Solver::Optimization::OptimizationOptions options_;
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    IResultWriter& writer_;
    std::reference_wrapper<Simulation::ISimulationObserver> simulationObserver_;
    std::shared_ptr<OptimisationsSimulationTable> simulationTables_;
};
} // namespace Antares::Solver::Optimization
