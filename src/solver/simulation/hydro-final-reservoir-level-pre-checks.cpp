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

namespace Antares
{
namespace Solver
{
void FinalReservoirLevelPreChecks(Data::Study& study)
{
    bool preChecksPasses = true;
    // Last Day of the simulation
    uint simEndDay = study.parameters.simulationDays.end;
    for (uint yearIndex = 0; yearIndex != study.scenarioFinalHydroLevels.height; ++yearIndex)
    {
        study.areas.each(
          [&](Data::Area& area)
          {
              // general data
              const Data::DataSeriesHydro& data = *area.hydro.series;
              int tsIndex = data.timeseriesNumbers[0][yearIndex];
              auto& inflowsmatrix = area.hydro.series->storage;
              auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];
              double initialReservoirLevel = study.scenarioInitialHydroLevels[area.index][yearIndex];
              double finalReservoirLevel = study.scenarioFinalHydroLevels[area.index][yearIndex];
              double deltaReservoirLevel = 0.0;
              int simEndRealMonth = 0;
              // FinalReservoirLevelRuntimeData
              auto& finLevData = area.hydro.finalReservoirLevelRuntimeData;
              finLevData.includeFinalReservoirLevel.push_back(false);
              finLevData.finResLevelMode.push_back(
                Antares::Data::FinalReservoirLevelMode::completeYear);
              finLevData.deltaLevel.push_back(deltaReservoirLevel);
              finLevData.endLevel.push_back(deltaReservoirLevel);
              finLevData.endMonthIndex.push_back(simEndRealMonth);

              if (area.hydro.reservoirManagement && !area.hydro.useWaterValue
                  && !isnan(finalReservoirLevel) && !isnan(initialReservoirLevel))
              {
                  // simEndDayReal
                  uint simEndDayReal = simEndDay;
                  // deltaReservoirLevel
                  deltaReservoirLevel = initialReservoirLevel - finalReservoirLevel;
                  // collect data for pre-checks
                  int initReservoirLvlMonth
                    = area.hydro.initializeReservoirLevelDate; // month [0-11]
                  int initReservoirLvlDay
                    = study.calendar.months[initReservoirLvlMonth].daysYear.first;
                  double reservoirCapacity = area.hydro.reservoirCapacity;
                  double totalYearInflows = 0.0;
                  // FinalReservoirLevelRuntimeData
                  finLevData.includeFinalReservoirLevel.at(yearIndex) = true;
                  finLevData.endLevel.at(yearIndex) = finalReservoirLevel;
                  finLevData.deltaLevel.at(yearIndex) = deltaReservoirLevel;
                  // select finResLevelMode
                  // Mode-1 
                  if (initReservoirLvlDay == 0 && simEndDay == DAYS_PER_YEAR)
                  {
                      finLevData.finResLevelMode.at(yearIndex)
                        = Antares::Data::FinalReservoirLevelMode::completeYear;
                  }
                  // Mode-2
                  else if (initReservoirLvlDay != 0 && simEndDay == DAYS_PER_YEAR)
                  {
                      simEndRealMonth = 0;
                      finLevData.finResLevelMode.at(yearIndex)
                        = Antares::Data::FinalReservoirLevelMode::incompleteYear;
                  }
                  // Mode-3/4
                  else
                  {
                      uint simEndMonth = study.calendar.days[simEndDay].month;
                      uint simEnd_MonthFirstDay = study.calendar.months[simEndMonth].daysYear.first;
                      uint simEnd_MonthLastDay = study.calendar.months[simEndMonth].daysYear.end;
                      // select month in which to reach final reservoir level (1st day of the selected month)
                      simEndRealMonth
                        = (simEndDay - simEnd_MonthFirstDay) <= (simEnd_MonthLastDay - simEndDay)
                            ? simEndMonth
                            : simEndMonth + 1;
                      finLevData.finResLevelMode.at(yearIndex)
                        = Antares::Data::FinalReservoirLevelMode::incompleteYear;

                      if (simEndRealMonth == 12 && initReservoirLvlDay == 0)
                      {
                          simEndRealMonth = 0;
                          finLevData.finResLevelMode.at(yearIndex)
                            = Antares::Data::FinalReservoirLevelMode::completeYear;
                      }
                      // E.g. End Date = 21.Dec && InitReservoirLevelDate = 1.Jan ->
                      // - > go back to first case
                      else if (simEndRealMonth == 12 && initReservoirLvlDay != 0)
                          simEndRealMonth = 0;
                      // End Date = 21.Dec && InitReservoirLevelDate = 1.Mar
                      // Reach FinalReservoirLevel at 1.Jan
                      else if (simEndRealMonth == initReservoirLvlMonth
                               && simEndDay >= initReservoirLvlDay)
                          simEndRealMonth = (simEndRealMonth + 1) % 12;
                      // E.g. End Date = 10.Jan && InitReservoirLevelDate = 1.Jan ->
                      // we need to move FinalReservoirLevel to 1.Feb.
                      // Cannot do both init and final on the same day
                      else if (simEndRealMonth == initReservoirLvlMonth
                               && simEndDay < initReservoirLvlDay)
                          simEndRealMonth = (simEndRealMonth - 1) % 12;
                      // E.g. End Date = 25.Nov && InitReservoirLevelDate = 1.Dec ->
                      // we need to move FinalReservoirLevel to 1.Nov.
                      // Cannot do both init and final on the same day

                      // log out that the final reservoir level will be reached on some other day
                      if (simEndDay != study.calendar.months[simEndRealMonth].daysYear.first)
                      {
                          simEndDayReal = study.calendar.months[simEndRealMonth].daysYear.first;
                          logs.info()
                            << "Year: " << yearIndex + 1 << ". Area: " << area.name
                            << ". Final reservoir level will be reached on day: " << simEndDayReal;
                      }
                  }
                  // Now convert to month if initialization is not done in January
                  int h20_solver_sim_end_month = (simEndRealMonth - initReservoirLvlMonth) >= 0
                                                   ? simEndRealMonth - initReservoirLvlMonth
                                                   : simEndRealMonth - initReservoirLvlMonth + 12;
                  finLevData.endMonthIndex.at(yearIndex) = h20_solver_sim_end_month;

                  // logs for debugging
                  logs.debug() << "yearIndex: " << yearIndex;
                  logs.debug() << "includeFinalReservoirLevel: "
                               << std::to_string(finLevData.includeFinalReservoirLevel.at(yearIndex));
                  logs.debug() << "finResLevelMode: "
                               << std::to_string(finLevData.finResLevelMode.at(yearIndex));
                  logs.debug() << "deltaLevel: " << finLevData.deltaLevel.at(yearIndex);
                  logs.debug() << "endLevel: " << finLevData.endLevel.at(yearIndex);
                  logs.debug() << "realMonth-SimEnd: " << simEndRealMonth;
                  logs.debug() << "endMonthIndex_h20_solver: " << finLevData.endMonthIndex.at(yearIndex);
                  logs.debug() << "simEndDayReal: " << simEndDayReal;
                  logs.debug() << "initReservoirLvlDay: " << initReservoirLvlDay;
                  // end of logs for debugging

                  // rule curve values for simEndDayReal
                  double lowLevelLastDay
                    = area.hydro.reservoirLevel[Data::PartHydro::minimum][simEndDayReal - 1];
                  double highLevelLastDay
                    = area.hydro.reservoirLevel[Data::PartHydro::maximum][simEndDayReal - 1];
                  // calculate (partial)yearly inflows
                  if (initReservoirLvlDay <= simEndDayReal)
                  {
                      for (uint day = initReservoirLvlDay; day < simEndDayReal; ++day)
                          totalYearInflows += srcinflows[day];
                  }
                  else
                  {
                      for (uint day = initReservoirLvlDay; day < DAYS_PER_YEAR; ++day)
                          totalYearInflows += srcinflows[day];
                      for (uint day = 0; day < simEndDayReal; ++day)
                          totalYearInflows += srcinflows[day];
                  }
                  // pre-check 1 -> reservoir_levelDay_365 – reservoir_levelDay_1 ≤
                  // yearly_inflows
                  if ((-deltaReservoirLevel) * reservoirCapacity
                      > totalYearInflows) // ROR time-series in MW (power), SP time-series in MWh
                                          // (energy)
                  {
                      logs.error() << "Year: " << yearIndex + 1 << ". Area: " << area.name
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
                      logs.error() << "Year: " << yearIndex + 1 << ". Area: " << area.name
                                   << ". Specifed final reservoir level: " << finalReservoirLevel
                                   << " is incompatible with reservoir level rule curve ["
                                   << lowLevelLastDay << " , " << highLevelLastDay << "]";
                      preChecksPasses = false;
                  }
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