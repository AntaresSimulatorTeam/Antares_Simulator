/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#ifndef __ORTOOLS_WRAPPER__
#define __ORTOOLS_WRAPPER__

#include <string>

#include <antares/optimization-options/options.h>

#include "named_problem.h"

using namespace operations_research;

MPSolver* ORTOOLS_Simplexe(Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                           MPSolver* ProbSpx,
                           bool keepBasis,
                           const Antares::Solver::Optimization::OptimizationOptions& options);

MPSolver* ConvertIntoOrtools(const std::string& solverName,
                             const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme);

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

#endif
