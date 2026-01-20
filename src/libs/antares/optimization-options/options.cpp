// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
