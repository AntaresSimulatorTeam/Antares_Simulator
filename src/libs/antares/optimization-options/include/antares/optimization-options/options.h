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

class OptimizationOptions final
{
public:
    void initializeWith(const CmdLineOptimOptions& cmdLineOptimOptions);

    SingleOptimOptions firstOptimOptions;
    SingleOptimOptions secondOptimOptions;
    SingleOptimOptions quadraticOptimOptions;
    bool solverLogs = false;
};
} // namespace Antares::Solver::Optimization
