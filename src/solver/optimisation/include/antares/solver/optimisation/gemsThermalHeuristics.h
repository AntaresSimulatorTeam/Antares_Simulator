// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optim-model-filler/VariableBoundOverrides.h>

namespace Antares::Solver
{
struct ModelerData;
}

namespace Antares::LinearProblem
{
class OptimEntityContainer;
}

namespace Antares::Optimization
{

/**
 * Per-week state of the thermal heuristics applied to GEMS components whose integer strategy is
 * "heuristic" (see integer-strategy in system.yml and heuristics in optim-config.yml).
 *
 * Workflow for each week:
 * 1. reset() before the first optimisation
 * 2. captureGemsHeuristicInputs() after each solve of the first optimisation (one solve per week,
 *    or one per day with a daily optimisation)
 * 3. runGemsThermalHeuristics() between the two optimisations: computes boundOverrides
 * 4. the second optimisation is built with boundOverrides (see ComponentFiller)
 *
 * One instance per PROBLEME_HEBDO, i.e. per thread.
 */
struct GemsHeuristicsState
{
    using VariableKey = std::pair<std::string, std::string>; // (component id, variable id)

    void reset(unsigned firstGlobalTimeStep, unsigned nbTimeSteps);

    unsigned firstGlobalTimeStep = 0;
    unsigned nbTimeSteps = 0;

    // Variable values of the first optimisation, indexed by time step of the week
    std::map<VariableKey, std::vector<double>> solutions;
    std::map<VariableKey, std::vector<double>> lowerBounds;
    std::map<VariableKey, std::vector<double>> upperBounds;

    // Bounds computed by the heuristics, applied when building the second optimisation
    LinearProblem::VariableBoundOverrides boundOverrides;
};

/// Stores the variable values needed by the heuristics, after a solve of the first optimisation
void captureGemsHeuristicInputs(GemsHeuristicsState& state,
                                const Solver::ModelerData& modelerData,
                                const LinearProblem::OptimEntityContainer& optimEntityContainer,
                                const LinearProblem::Api::FillContext& fillContext);

/// Runs the heuristics of all GEMS components handled by a heuristic and fills
/// state.boundOverrides
void runGemsThermalHeuristics(GemsHeuristicsState& state,
                              const Solver::ModelerData& modelerData,
                              unsigned year);

} // namespace Antares::Optimization
