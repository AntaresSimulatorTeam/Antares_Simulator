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

#include "hydro-final-reservoir-level-pre-checks.h"
#include <antares/emergency.h>
// #include "../solver/simulation/sim_extern_variables_globales.h"

namespace Antares
{
namespace Solver
{
void FinalReservoirLevelPreChecks(Data::Study& study)
{
    bool preChecksPasses = true;
    for (uint tsIndex = 0; tsIndex != study.scenarioFinalHydroLevels.height; ++tsIndex)
    {
        study.areas.each(
          [&](Data::Area& area)
          {
              double deltaReservoirLevel = 0.0;
              // TODO CR25:
              /*at this point the pre-checks are done for all MC before running the simulation
              and the simulation is ended immediately not waisting user time!
              However, at this point "tsIndex = (uint)ptchro.Hydraulique" is not assigned yet
              so the pre-checks are done for all MC years (whether they are used or not in hydro
              scenario-builder) this can lead to error reporting for MC years that are not used (not
              the case when pre-checks done im management.cpp)
              */
              // auto& ptchro = *NumeroChroniquesTireesParPays[numSpace][area.index];
              auto& inflowsmatrix = area.hydro.series->storage;
              // auto tsIndex = (uint)ptchro.Hydraulique;
              auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];
              double initialReservoirLevel = study.scenarioHydroLevels[area.index][tsIndex];
              double finalReservoirLevel = study.scenarioFinalHydroLevels[area.index][tsIndex];

              if (area.hydro.reservoirManagement && !area.hydro.useWaterValue
                  && !isnan(finalReservoirLevel) && !isnan(initialReservoirLevel))
              {
                  // deltaReservoirLevel
                  deltaReservoirLevel = initialReservoirLevel - finalReservoirLevel;
                  // collect data for pre-checks
                  double reservoirCapacity = area.hydro.reservoirCapacity;
                  double lowLevelLastDay
                    = area.hydro.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
                  double highLevelLastDay
                    = area.hydro.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];
                  double totalYearInflows = 0.0;
                  // calculate yearly inflows
                  for (uint day = 0; day < DAYS_PER_YEAR; ++day)
                  {
                      totalYearInflows += srcinflows[day];
                  }
                  // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
                  // yearly_inflows
                  if ((finalReservoirLevel - initialReservoirLevel) * reservoirCapacity
                      > totalYearInflows) // ROR time-series in MW (power), SP time-series in MWh (energy)
                  {
                      logs.error() << "Year: " << tsIndex + 1 << ". Area: " << area.name
                                   << ". Incompatible total inflows: " << totalYearInflows
                                   << " with initial: " << initialReservoirLevel
                                   << " and final: " << finalReservoirLevel << " reservoir levels.";
                      preChecksPasses = false;
                  }
                  // pre-check 2 -> final reservoir level set by the user is within the
                  // rule curves for the final day
                  if (finalReservoirLevel < lowLevelLastDay
                      || finalReservoirLevel > highLevelLastDay)
                  {
                      logs.error() << "Year: " << tsIndex + 1 << ". Area: " << area.name
                                   << ". Specifed final reservoir level: " << finalReservoirLevel
                                   << " is incompatible with reservoir level rule curve ["
                                   << lowLevelLastDay << " , " << highLevelLastDay << "]";
                      preChecksPasses = false;
                  }
              }
              area.hydro.finalReservoirLevelCorrection.push_back(deltaReservoirLevel);
          });
    }
    if (!preChecksPasses)
    {
        logs.fatal() << "At least one year has failed final reservoir level pre-checks.";
        AntaresSolverEmergencyShutdown();
    }
}

} // namespace Solver
} // namespace Antares