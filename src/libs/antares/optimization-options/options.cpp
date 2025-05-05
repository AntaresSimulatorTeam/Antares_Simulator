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
#include "antares/optimization-options/options.h"

namespace Antares::Solver::Optimization
{
void OptimizationOptions::initializeWith(const CmdLineOptimOptions& cmdLineOptimOptions)
{
    // Do solvers log their own messaqes
    solverLogs = cmdLineOptimOptions.solverLogs || solverLogs;
    firstOptimOptions.solverLogs = solverLogs;
    secondOptimOptions.solverLogs = solverLogs;
    quadraticOptimOptions.solverLogs = solverLogs;

    // Solver names
    firstOptimOptions.solverName = cmdLineOptimOptions.linearSolver;
    secondOptimOptions.solverName = cmdLineOptimOptions.linearSolver;
    quadraticOptimOptions.solverName = cmdLineOptimOptions.quadraticSolver;

    // Storing basis or giving solver a basis, depending on optimization (first or second)
    firstOptimOptions.solverUsesBasis = cmdLineOptimOptions.useOptim1BasisInNextWeek;
    firstOptimOptions.solverExportsBasis = cmdLineOptimOptions.useOptim1BasisInNextWeek
                                           || cmdLineOptimOptions.useOptim1BasisInOptim2;
    secondOptimOptions.solverUsesBasis = cmdLineOptimOptions.useOptim1BasisInOptim2;

    // Linear solver parameters
    if (cmdLineOptimOptions.linearSolverParameters.empty())
    {
        firstOptimOptions.solverParameters = cmdLineOptimOptions.lpSolverParamOptim1;
        secondOptimOptions.solverParameters = cmdLineOptimOptions.lpSolverParamOptim2;
    }
    else
    {
        firstOptimOptions.solverParameters = cmdLineOptimOptions.linearSolverParameters;
        secondOptimOptions.solverParameters = cmdLineOptimOptions.linearSolverParameters;
    }

    // Quadratic solver parameters
    quadraticOptimOptions.solverParameters = cmdLineOptimOptions.quadraticSolverParameters;
}
} // namespace Antares::Solver::Optimization
