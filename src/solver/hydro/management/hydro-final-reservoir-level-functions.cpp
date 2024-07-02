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

#include "antares/solver/hydro/management/hydro-final-reservoir-level-functions.h"

#include <antares/antares/fatal-error.h>
#include "antares/study/parts/hydro/finalLevelValidator.h"

namespace Antares::Solver
{

void CheckFinalReservoirLevelsConfiguration(Data::AreaList& areas,
                                            const Data::Parameters& parameters,
                                            const Data::TimeSeries::TS& scenarioInitialHydroLevels,
                                            const Data::TimeSeries::TS& scenarioFinalHydroLevels,
                                            uint year)
{
    if (!parameters.yearsFilter.at(year))
    {
        return;
    }

    areas.each(
      [&areas, &parameters, &scenarioInitialHydroLevels, &scenarioFinalHydroLevels, year](
        Data::Area& area)
      {
          double initialLevel = scenarioInitialHydroLevels.entry[area.index][year];
          double finalLevel = scenarioFinalHydroLevels.entry[area.index][year];

          Data::FinalLevelValidator validator(area.hydro,
                                              area.index,
                                              area.name,
                                              initialLevel,
                                              finalLevel,
                                              year,
                                              parameters.simulationDays.end,
                                              parameters.firstMonthInYear);
          if (!validator.check())
          {
              throw FatalError("hydro final level : infeasibility");
          }
          if (validator.finalLevelFineForUse())
          {
              area.hydro.deltaBetweenFinalAndInitialLevels[year] = finalLevel - initialLevel;
          }
      });
} // End function CheckFinalReservoirLevelsConfiguration

} // namespace Antares::Solver
