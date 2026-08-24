// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

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

    // Every stage name, in the order the weekly resolution reaches them.
    static const std::vector<std::string>& allStages();

    // Parses a user-supplied stage list: comma-separated names, or "all". An
    // empty input also means every stage, and "all" anywhere in the list widens
    // it to every stage. Throws std::runtime_error naming the valid stages when
    // a name is not one of them -- including names that follow an "all", so the
    // whole list is checked whatever it ends up meaning; `source` is how that
    // message refers to where the list came from, since it can be either the
    // command line or generaldata.ini.
    static std::set<std::string> parseStageSelection(
      const std::string& input,
      const std::string& source = "--simulation-table-stages");

    // Restricts the tables to `stages`. An empty set means no restriction, so
    // the default is to produce every stage. Call before the first dump: stages
    // already created are not removed.
    void selectStages(std::set<std::string> stages);

    SimulationTable* firstOptimSimulationTable();
    SimulationTable* secondOptimSimulationTable();

    // The table of `stage`, created empty on first use, or nullptr when the
    // stage is not selected — callers must skip the dump on nullptr. std::map
    // nodes are address-stable, so a pointer returned here stays valid when
    // later stages are added.
    SimulationTable* tableForStage(const std::string& stage);

    // Whether `stage` would get a table, without creating one.
    [[nodiscard]] bool isStageSelected(const std::string& stage) const;

    // Whether any stage dumped after a post-process is selected. The weekly
    // solve asks this to decide whether it must keep its modeler problem alive
    // for a later dump to re-emit: nothing reads it when only the optimisation
    // stages are wanted.
    [[nodiscard]] bool anyPostProcessStageSelected() const;

    [[nodiscard]] const std::map<std::string, SimulationTable>& stages() const;

    // Empties every stage's table, keeping the stages themselves: the same
    // stages recur at every Monte-Carlo year.
    void clear();

private:
    std::map<std::string, SimulationTable> stages_;
    // Empty means "every stage", which is what an unrestricted run wants.
    std::set<std::string> selectedStages_;
};
} // namespace Antares::IO::Outputs
