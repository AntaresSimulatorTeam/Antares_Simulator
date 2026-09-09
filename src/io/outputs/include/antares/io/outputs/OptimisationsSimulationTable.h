// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <set>
#include <string>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/io/outputs/SimulationTableStage.h"

namespace Antares::Optimization
{
class InactiveComponentsAnalyzer;
}

namespace Antares::IO::Outputs
{
/**
 * \brief The simulation tables of one Monte-Carlo year, one per stage of the
 * weekly resolution.
 *
 * A stage is created on first use; the writer emits one file per stage. See
 * docs/architecture/legacy-extra-outputs.md §8.
 */
class OptimisationsSimulationTable
{
public:
    /**
     * \brief Parse a user-supplied stage list into a selection set.
     * \param input comma-separated stage names, or "all". Empty, or "all"
     *              anywhere in the list, selects every stage.
     * \param source how the error message names the origin of the list (command
     *               line or generaldata.ini).
     * \return the selected stages; an empty set means "every stage".
     * \throws Antares::Error::InvalidArgumentError, listing the valid names, on
     *         an unknown name -- including one after an "all", as the whole list
     *         is validated.
     */
    static std::set<Stage> parseStageSelection(
      const std::string& input,
      const std::string& source = "--simulation-table-stages");

    /**
     * \brief Restrict the tables to `stages`; an empty set lifts the restriction.
     *
     * Call before the first dump: already-created stages are not removed.
     */
    void selectStages(std::set<Stage> stages);

    SimulationTable* firstOptimSimulationTable();
    SimulationTable* secondOptimSimulationTable();

    /**
     * \brief The table of `stage`, created empty on first use.
     * \return nullptr when the stage is not selected -- callers must then skip
     *         the dump. Returned pointers stay valid as later stages are added.
     */
    SimulationTable* tableForStage(Stage stage);

    /// \brief Whether `stage` would get a table, without creating one.
    [[nodiscard]] bool isStageSelected(Stage stage) const;

    /**
     * \brief Whether any stage dumped after a post-process is selected.
     *
     * The weekly solve asks this to decide whether to keep its modeler problem
     * alive for a later dump to re-emit.
     */
    [[nodiscard]] bool anyPostProcessStageSelected() const;

    [[nodiscard]] const std::map<Stage, SimulationTable>& stages() const;

    /// \brief Empty every stage's table, keeping the stages: they recur every
    /// Monte-Carlo year.
    void clear();

    std::shared_ptr<const Optimization::InactiveComponentsAnalyzer> inactiveComponents;

private:
    std::map<Stage, SimulationTable> stages_;
    /// Empty means "every stage".
    std::set<Stage> selectedStages_;
};
} // namespace Antares::IO::Outputs
