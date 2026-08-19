// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>

#include "antares/io/outputs/SimulationTable.h"

namespace Antares::IO::Outputs
{
// The simulation tables produced for one Monte-Carlo year, one per stage of the
// weekly resolution. A stage is created the first time it is asked for, and the
// writer emits one file per stage, named after it.
class OptimisationsSimulationTable
{
public:
    // Stage names. They are part of the output file names, so they must not
    // change. The first two are produced during the weekly solve, the others
    // after a post-process has moved the results.
    static constexpr const char* firstOptimStage = "optim-nb-1";
    static constexpr const char* secondOptimStage = "optim-nb-2";
    static constexpr const char* remixHydroStage = "remix-hydro";
    // The whole CSR treatment, not just the patch: curtailment sharing, DTG
    // netting and the marginal price update.
    static constexpr const char* adequacyPatchStage = "adq-patch-csr";

    SimulationTable* firstOptimSimulationTable();
    SimulationTable* secondOptimSimulationTable();

    // The table of `stage`, created empty on first use. std::map nodes are
    // address-stable, so a pointer returned here stays valid when later stages
    // are added.
    SimulationTable* tableForStage(const std::string& stage);

    [[nodiscard]] const std::map<std::string, SimulationTable>& stages() const;

    // Empties every stage's table, keeping the stages themselves: the same
    // stages recur at every Monte-Carlo year.
    void clear();

private:
    std::map<std::string, SimulationTable> stages_;
};
} // namespace Antares::IO::Outputs
