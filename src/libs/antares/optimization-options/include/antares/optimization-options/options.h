// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <string>

namespace Antares::Solver::Optimization
{
struct SingleOptimOptions
{
    std::string solverName = "sirius";
    std::string solverParameters;

    // Reusing basis of first optimization (in case we have 2 weekly
    // linear optimizations [not MILP])
    bool solverUsesBasis = false;
    bool solverExportsBasis = false;

    bool solverLogs = false;
};

struct CmdLineOptimOptions
{
    std::string linearSolver = "sirius";
    std::string linearSolverParameters;
    std::string lpSolverParamOptim1;
    std::string lpSolverParamOptim2;
    bool useOptim1BasisInNextWeek = true;
    bool useOptim1BasisInOptim2 = true;
    std::string quadraticSolver = "sirius";
    std::string quadraticSolverParameters;
    bool solverLogs = false;
};

enum class ExportBehavior
{
    // Never export structure problems
    Never,
    // Export once, on the first call
    Once,
    // Always export the structure problems
    Always
};

class OptimizationOptions final
{
public:
    void initializeWith(const CmdLineOptimOptions& cmdLineOptimOptions);

    SingleOptimOptions firstOptimOptions;
    SingleOptimOptions secondOptimOptions;
    SingleOptimOptions quadraticOptimOptions;
    bool solverLogs = false;
    ExportBehavior exportBehavior = ExportBehavior::Never;
};
} // namespace Antares::Solver::Optimization
