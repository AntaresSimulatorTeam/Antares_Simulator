/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
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
