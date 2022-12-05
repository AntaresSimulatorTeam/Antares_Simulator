/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
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
    averageOptimizationTime = 0.;
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
    scratchpad = area->scratchpad[numSpace];
    thermalCluster = nullptr;
    timeseriesIndex = NumeroChroniquesTireesParPays[numSpace][areaIndex];

    switch (studyMode)
    {
    case Data::stdmEconomy:
    {
        hourlyResults = problemeHebdo->ResultatsHoraires[areaIndex];
        break;
    }
    case Data::stdmAdequacy:
    {
        hourlyResults = problemeHebdo->ResultatsHoraires[areaIndex];
        break;
    }
    case Data::stdmAdequacyDraft:
    {
        hourlyAdequacyResults = ProblemeHoraireAdequation.ResultatsParPays[areaIndex];
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
