/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_VARIABLE_STATE_HXX__
#define __SOLVER_VARIABLE_STATE_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
inline void State::startANewYear()
{
    hourInTheSimulation = 0u;

    memset(thermalClusterProductionForYear, 0, sizeof(thermalClusterProductionForYear));
    memset(thermalClusterOperatingCostForYear, 0, sizeof(thermalClusterOperatingCostForYear));
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));

    // Re-initializing annual costs (to be printed in output into separate files)
    annualSystemCost = 0.;
    optimalSolutionCost1 = 0.;
    optimalSolutionCost2 = 0.;
    averageOptimizationTime1 = 0.;
    averageOptimizationTime2 = 0.;
}

inline void State::yearEndResetThermal()
{
    memset(thermalClusterProductionForYear, 0, sizeof(thermalClusterProductionForYear));
    memset(thermalClusterOperatingCostForYear, 0, sizeof(thermalClusterOperatingCostForYear));
    memset(thermalClusterNonProportionalCostForYear,
           0,
           sizeof(thermalClusterNonProportionalCostForYear));
    memset(thermalClusterPMinOfTheClusterForYear, 0, sizeof(thermalClusterPMinOfTheClusterForYear));
    memset(thermalClusterDispatchedUnitsCountForYear,
           0,
           sizeof(thermalClusterDispatchedUnitsCountForYear));
}

inline void State::initFromAreaIndex(const unsigned int areaIndex, uint numSpace)
{
    area = study.areas[areaIndex];
    scratchpad = &area->scratchpad[numSpace];
    thermalCluster = nullptr;

    switch (studyMode)
    {
    case Data::stdmEconomy:
    {
        hourlyResults = &problemeHebdo->ResultatsHoraires[areaIndex];
        break;
    }
    case Data::stdmAdequacy:
    {
        hourlyResults = &problemeHebdo->ResultatsHoraires[areaIndex];
        break;
    }
    case Data::stdmUnknown:
        break;
    case Data::stdmExpansion:
        break;
    case Data::stdmMax:
        break;
    }
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STATE_HXX__
