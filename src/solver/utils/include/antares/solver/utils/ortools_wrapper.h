// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimization-options/options.h>
#include <antares/solver/optimisation/opt_structure_probleme_a_resoudre.h>

using namespace operations_research;

void ORTOOLS_Simplexe(PROBLEME_ANTARES_A_RESOUDRE* problemeHebdo,
                      MPSolver* ProbSpx,
                      const Antares::Solver::Optimization::SingleOptimOptions& options);
