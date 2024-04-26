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

#pragma once
#include <antares/logs/logs.h>

namespace Antares::Solver::Optimization
{

class SolverParameters
{
public:
    std::string xpress;
    std::string scip;

    void overrideByOtherIfNotEmpty(const SolverParameters& other)
    {
        xpress = overrideParameterStringByOtherIfNotEmpty(xpress, other.xpress);
        displayParameterString(xpress, "xpress");

        scip = overrideParameterStringByOtherIfNotEmpty(scip, other.scip);
        displayParameterString(scip, "scip");
    }

private:
    static std::string overrideParameterStringByOtherIfNotEmpty(
      const std::string& srcParameters,
      const std::string& otherParameters)
    {
        if (!otherParameters.empty())
        {
            return otherParameters;
        }
        return srcParameters;
    }

    static void displayParameterString(const std::string& parameterString,
                                       const std::string& solverName)
    {
        if (!parameterString.empty())
        {
            logs.info() << " " + solverName + " solver specific parameters: " << parameterString;
        }
    }
};

struct OptimizationOptions
{
    //! Force ortools use
    bool ortoolsUsed = false;
    //! The solver name, sirius is the default
    std::string ortoolsSolver = "sirius";
    bool solverLogs = false;
    SolverParameters solverParameters;
};
} // namespace Antares::Solver::Optimization
