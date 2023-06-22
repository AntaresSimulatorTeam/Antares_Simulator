/*
** Copyright 2007-2023 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include "hydro-final-reservoir-level-functions.h"
#include <antares/emergency.h>

namespace Antares::Solver
{
static void initializeGeneralData(Data::FinalReservoirLevelRuntimeData& finLevData,
                           const Data::Parameters& parameters,
                           uint year)
{
    finLevData.simEndDay = parameters.simulationDays.end;
    finLevData.yearIndex = year;
}

static void initializePerAreaData(Data::FinalReservoirLevelRuntimeData& finLevData,
                           const Matrix<double>& scenarioInitialHydroLevels,
                           const Matrix<double>& scenarioFinalHydroLevels,
                           const Data::Area& area)
{
    finLevData.initialReservoirLevel = scenarioInitialHydroLevels[area.index][finLevData.yearIndex];
    finLevData.finalReservoirLevel = scenarioFinalHydroLevels[area.index][finLevData.yearIndex];
    finLevData.deltaReservoirLevel
      = finLevData.initialReservoirLevel - finLevData.finalReservoirLevel;
}

void initializePreCheckData(Data::FinalReservoirLevelRuntimeData& finLevData,
                            const Data::Area& area)
{
    finLevData.initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate; // month [0-11]
    finLevData.reservoirCapacity = area.hydro.reservoirCapacity;
}

void ruleCurveForSimEndReal(Data::FinalReservoirLevelRuntimeData& finLevData, Data::Area& area)
{
    finLevData.lowLevelLastDay
      = area.hydro.reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    finLevData.highLevelLastDay
      = area.hydro.reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];
}

void FinalReservoirLevel(const Matrix<double>& scenarioInitialHydroLevels,
                         const Matrix<double>& scenarioFinalHydroLevels,
                         const Data::Parameters& parameters,
                         const Data::AreaList& areas)
{
    bool preChecksPasses = true;
    for (uint yearIndex = 0; yearIndex != scenarioFinalHydroLevels.height; ++yearIndex)
    {
        areas.each(
          [&scenarioInitialHydroLevels,
           &scenarioFinalHydroLevels,
           &parameters,
           &preChecksPasses,
           &yearIndex](Data::Area& area)
          {
              auto& finLevData = area.hydro.finalReservoirLevelRuntimeData;
              finLevData.fillEmpty();
              initializeGeneralData(finLevData, parameters, yearIndex);
              initializePerAreaData(
                finLevData, scenarioInitialHydroLevels, scenarioFinalHydroLevels, area);

              if (area.hydro.reservoirManagement && !area.hydro.useWaterValue
                  && !isnan(finLevData.finalReservoirLevel)
                  && !isnan(finLevData.initialReservoirLevel))
              {
                  initializePreCheckData(finLevData, area);
                  finLevData.assignEndLevelAndDelta();

                  // pre-check 0 -> simulation must end on day 365 and reservoir level must be
                  // initiated in January
                  if (!finLevData.preCheckStartAndEndSim(area.name))
                      preChecksPasses = false;

                  // rule curve values for simEndDayReal
                  ruleCurveForSimEndReal(finLevData, area);

                  // calculate (partial)yearly inflows
                  const Data::DataSeriesHydro& data = *area.hydro.series;
                  uint tsHydroIndex = data.timeseriesNumbers[0][finLevData.yearIndex];
                  auto& inflowsmatrix = area.hydro.series->storage;
                  auto& srcinflows
                    = inflowsmatrix[tsHydroIndex < inflowsmatrix.width ? tsHydroIndex : 0];

                  // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
                  // yearly_inflows
                  if (double totalInflows = finLevData.calculateTotalInflows(srcinflows);
                      !finLevData.preCheckYearlyInflow(totalInflows, area.name))
                      preChecksPasses = false;

                  // pre-check 2 -> final reservoir level set by the user is within the
                  // rule curves for the final day
                  if (!finLevData.preCheckRuleCurves(area.name))
                      preChecksPasses = false;
              }
          });
    }
    if (!preChecksPasses)
    {
        logs.fatal() << "At least one year has failed final reservoir level pre-checks.";
        AntaresSolverEmergencyShutdown();
    }
}

} // namespace Anatres::Solver