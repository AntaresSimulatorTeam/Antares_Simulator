/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <antares/optimization-options/options.h>
#include <antares/solver/optimisation/opt_structure_probleme_a_resoudre.h>

using namespace operations_research;

MPSolver* ORTOOLS_Simplexe(PROBLEME_ANTARES_A_RESOUDRE* problemeHebdo,
                           MPSolver* ProbSpx,
                           const Antares::Solver::Optimization::SingleOptimOptions& options);

void ORTOOLS_ModifierLeVecteurCouts(MPSolver* ProbSpx, const double* costs, int nbVar);
void ORTOOLS_ModifierLeVecteurSecondMembre(MPSolver* ProbSpx,
                                           const double* rhs,
                                           const char* sens,
                                           int nbRow);
void ORTOOLS_CorrigerLesBornes(MPSolver* ProbSpx,
                               const double* bMin,
                               const double* bMax,
                               const int* typeVar,
                               int nbVar);
void ORTOOLS_LibererProbleme(MPSolver* ProbSpx);
