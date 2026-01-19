// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimization-options/options.h>
#include <antares/solver/optimisation/opt_structure_probleme_a_resoudre.h>

/**
 * This wrapper is an adapter that solves a QP stored in a PROBLEME_ANTARES_A_RESOUDRE
 * using OR-Tools MathOpt API & solvers.
 * Currently, QP support in MathOpt is implemented for Gurobi and SIP (though SCIP in OR-Tools
 * has compilation issues), and under development for XPRESS.
 */
void SolveQuadraticProblemWithOrtools(
  const Antares::Solver::Optimization::SingleOptimOptions& options,
  PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre);
