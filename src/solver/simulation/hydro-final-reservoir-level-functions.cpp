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

#include "hydro-final-reservoir-level-functions.h"
#include <antares/emergency.h>

namespace Antares
{
namespace Solver
{
void initializeGeneralData(Data::FinalReservoirLevelRuntimeData& finLevData,
                           Data::Study& study,
                           uint year)
{
    finLevData.simEndDay = study.parameters.simulationDays.end;
    finLevData.simEndMonth = study.calendar.days[finLevData.simEndDay].month;
    finLevData.simEndMonth_FirstDay = study.calendar.months[finLevData.simEndMonth].daysYear.first;
    finLevData.simEndMonth_LastDay = study.calendar.months[finLevData.simEndMonth].daysYear.end;
    finLevData.yearIndex = year;
}

void initializePerAreaData(Data::FinalReservoirLevelRuntimeData& finLevData,
                           Data::Study& study,
                           Data::Area& area)
{
    finLevData.initialReservoirLevel
      = study.scenarioInitialHydroLevels[area.index][finLevData.yearIndex];
    finLevData.finalReservoirLevel
      = study.scenarioFinalHydroLevels[area.index][finLevData.yearIndex];
    finLevData.deltaReservoirLevel
      = finLevData.initialReservoirLevel - finLevData.finalReservoirLevel;
}

void initializePreCheckData(Data::FinalReservoirLevelRuntimeData& finLevData,
                            Data::Study& study,
                            Data::Area& area)
{
    finLevData.initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate; // month [0-11]
    finLevData.initReservoirLvlDay
      = study.calendar.months[finLevData.initReservoirLvlMonth].daysYear.first;
    finLevData.reservoirCapacity = area.hydro.reservoirCapacity;
}

void ruleCurveForSimEndReal(Data::FinalReservoirLevelRuntimeData& finLevData,
                            uint simEndDayReal,
                            Data::Area& area)
{
    finLevData.lowLevelLastDay
      = area.hydro.reservoirLevel[Data::PartHydro::minimum][simEndDayReal - 1];
    finLevData.highLevelLastDay
      = area.hydro.reservoirLevel[Data::PartHydro::maximum][simEndDayReal - 1];
}

void FinalReservoirLevel(Data::Study& study)
{
    bool preChecksPasses = true;
    for (uint yearIndex = 0; yearIndex != study.scenarioFinalHydroLevels.height; ++yearIndex)
    {
        study.areas.each(
          [&](Data::Area& area)
          {
              auto& finLevData = area.hydro.finalReservoirLevelRuntimeData;
              finLevData.fillEmpty();
              initializeGeneralData(finLevData, study, yearIndex);
              initializePerAreaData(finLevData, study, area);

              if (area.hydro.reservoirManagement && !area.hydro.useWaterValue
                  && !isnan(finLevData.finalReservoirLevel)
                  && !isnan(finLevData.initialReservoirLevel))
              {
                  initializePreCheckData(finLevData, study, area);
                  finLevData.assignEndLevelAndDelta();

                  // determine simEnd month and FinalReservoirLevelMode
                  int simEndRealMonth = finLevData.selectMode();

                  // log if the final reservoir level will be reached on some other day
                  uint simEndDayReal = study.calendar.months[simEndRealMonth].daysYear.first;
                  if (simEndDayReal == 0)
                      simEndDayReal = DAYS_PER_YEAR;
                  finLevData.logSimEndDayChange(simEndDayReal, area.name);

                  // Now convert to h20-solver-month if initialization is not done in January
                  finLevData.shiftMonths(simEndRealMonth);

                  // rule curve values for simEndDayReal
                  ruleCurveForSimEndReal(finLevData, simEndDayReal, area);

                  // calculate (partial)yearly inflows
                  const Data::DataSeriesHydro& data = *area.hydro.series;
                  int tsHydroIndex = data.timeseriesNumbers[0][finLevData.yearIndex];
                  auto& inflowsmatrix = area.hydro.series->storage;
                  auto& srcinflows
                    = inflowsmatrix[tsHydroIndex < inflowsmatrix.width ? tsHydroIndex : 0];
                  double totalInflows = finLevData.calculateTotalInflows(srcinflows, simEndDayReal);

                  // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
                  // yearly_inflows
                  if (!finLevData.preCheckOne(totalInflows, area.name))
                      preChecksPasses = false;

                  // pre-check 2 -> final reservoir level set by the user is within the
                  // rule curves for the final day
                  if (!finLevData.preCheckTwo(area.name))
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

} // namespace Solver
} // namespace Antares