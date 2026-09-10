// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace Antares::Solver
{

/// Error thrown by the modeler when the scenario scope selection is invalid
class InvalidScenarioScopeError: public std::runtime_error
{
public:
    explicit InvalidScenarioScopeError(const std::string& s):
        std::runtime_error(s)
    {
    }
};

/** \brief Selects which Monte-Carlo scenarios to simulate.
 *
 * The base scenario set is given by the inline include list. exclude is optional and
 * applies to that set.
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
};

/** \brief Resolve a scenario scope into the concrete, sorted, deduplicated list of scenario
 * indices to simulate.
 *
 * Implements the rules described in the parameters.yml documentation:
 *  - the base set comes from the inline include list;
 *  - excludes are removed from the base set (with a warning when they have no effect);
 *  - indices are sorted in ascending order;
 *  - an empty scenario scope falls back to scenario 0 only.
 */
std::vector<unsigned> resolveScenarioScopeScenarios(const ScenarioScope& scope);

} // namespace Antares::Solver
