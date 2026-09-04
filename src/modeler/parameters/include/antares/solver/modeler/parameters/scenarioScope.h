// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Antares::Solver
{

/** \brief Selects which Monte-Carlo scenarios to simulate.
 *
 * The base scenario set is defined by exactly one of two mutually exclusive keys:
 * include (inline) or playlistFile (from a JSON file). exclude is optional and applies
 * to either form.
 *
 * Indices are 0-based, consistent with the modeler-scenariobuilder.dat convention.
 * An empty scenarioScope (or one that resolves to an empty set of selected scenarios)
 * falls back to running scenario 0 only.
 */
struct ScenarioScope
{
    // Inline form: individual integers, string integers, and inclusive "a-b" range strings.
    std::vector<std::string> include;
    // Scenarios to remove from the base set (optional).
    std::vector<std::string> exclude;
    // A JSON file (optional): a list of integers / string integers / "a-b" range strings.
    std::optional<std::filesystem::path> playlistFile;

    bool operator==(const ScenarioScope& other) const = default;
    bool operator!=(const ScenarioScope& other) const = default;
};

/** \brief Resolve a scenario scope into the concrete, sorted, deduplicated list of scenario
 * indices to simulate.
 *
 * Implements the rules described in the parameters.yml documentation:
 *  - the base set comes from either the inline include list or the JSON playlist file;
 *  - excludes are removed from the base set (with a warning when they have no effect);
 *  - indices are sorted in ascending order;
 *  - an empty scenario scope falls back to scenario 0 only.
 */
std::vector<unsigned> resolveScenarioScopeScenarios(const ScenarioScope& scope,
                                                    const std::filesystem::path& studyPath);

} // namespace Antares::Solver
