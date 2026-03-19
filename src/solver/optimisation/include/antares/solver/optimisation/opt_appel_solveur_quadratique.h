// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/optimization-options/options.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique(PROBLEME_HEBDO*);
void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLeSecondMembreDuProblemeQuadratique(PROBLEME_HEBDO*, int);
void OPT_InitialiserLesCoutsQuadratiques(PROBLEME_HEBDO*, int);
bool OPT_AppelDuSolveurQuadratique(const Solver::Optimization::SingleOptimOptions& options,
                                   PROBLEME_ANTARES_A_RESOUDRE*);
bool OPT_PilotageOptimisationQuadratique(const Solver::Optimization::SingleOptimOptions& options,
                                         PROBLEME_HEBDO*);
