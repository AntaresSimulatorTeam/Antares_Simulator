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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <antares/study/study.h>
#include <antares/study/memory-usage.h>
#include "common-eco-adq.h"
#include <antares/logs.h>
#include <cassert>
#include "simulation.h"
#include <antares/study/area/scratchpad.h>
#include <antares/study/parts/hydro/container.h>

namespace Antares
{
namespace Solver
{
namespace Simulation
{
void computingHydroLevels(const Data::Study& study,
                          PROBLEME_HEBDO& problem,
                          uint nbHoursInAWeek,
                          bool remixWasRun,
                          bool computeAnyway)
{
    assert(study.parameters.mode != Data::stdmAdequacyDraft);

    study.areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        if (not computeAnyway)
        {
            if (area.hydro.useHeuristicTarget && !remixWasRun)
                return;

            if (!area.hydro.useHeuristicTarget && remixWasRun)
                return;
        }

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        double* inflows = problem.CaracteristiquesHydrauliques[index]->ApportNaturelHoraire;

        RESULTATS_HORAIRES* weeklyResults = problem.ResultatsHoraires[index];

        double* turb = weeklyResults->TurbinageHoraire;

        double* pump = weeklyResults->PompageHoraire;
        double pumpingRatio = area.hydro.pumpingEfficiency;

        double nivInit = problem.CaracteristiquesHydrauliques[index]->NiveauInitialReservoir;
        double* niv = weeklyResults->niveauxHoraires;

        double* ovf = weeklyResults->debordementsHoraires;

        auto& computeLvlObj = problem.computeLvl_object;

        computeLvlObj.setParameters(
          nivInit, inflows, ovf, turb, pumpingRatio, pump, reservoirCapacity);

        for (uint h = 0; h < nbHoursInAWeek - 1; h++)
        {
            computeLvlObj.run();
            niv[h] = computeLvlObj.getLevel() * 100 / reservoirCapacity;
            computeLvlObj.prepareNextStep();
        }

        computeLvlObj.run();
        niv[nbHoursInAWeek - 1] = computeLvlObj.getLevel() * 100 / reservoirCapacity;
    });
}

void interpolateWaterValue(const Data::Study& study,
                           PROBLEME_HEBDO& problem,
                           Antares::Solver::Variable::State& state,
                           int firstHourOfTheWeek,
                           uint nbHoursInAWeek)
{
    uint daysOfWeek[7] = {0, 0, 0, 0, 0, 0, 0};

    const uint weekFirstDay = study.calendar.hours[firstHourOfTheWeek].dayYear;

    daysOfWeek[0] = weekFirstDay;
    for (int d = 1; d < 7; d++)
        daysOfWeek[d] = weekFirstDay + d;

    study.areas.each([&](const Data::Area& area) {
        uint index = area.index;

        RESULTATS_HORAIRES* weeklyResults = problem.ResultatsHoraires[index];

        double* waterVal = weeklyResults->valeurH2oHoraire;

        for (uint h = 0; h < nbHoursInAWeek; h++)
            waterVal[h] = 0.;

        if (!area.hydro.reservoirManagement || !area.hydro.useWaterValue)
            return;

        if (!area.hydro.useWaterValue)
            return;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        double* niv = weeklyResults->niveauxHoraires;

        Antares::Data::getWaterValue(
          problem.previousSimulationFinalLevel[index] * 100 / reservoirCapacity,
          area.hydro.waterValues,
          weekFirstDay,
          state.h2oValueWorkVars,
          waterVal[0]);
        for (uint h = 1; h < nbHoursInAWeek; h++)
            Antares::Data::getWaterValue(niv[h - 1],
                                         area.hydro.waterValues,
                                         daysOfWeek[h / 24],
                                         state.h2oValueWorkVars,
                                         waterVal[h]);
    });
}

void updatingWeeklyFinalHydroLevel(const Data::Study& study,
                                   PROBLEME_HEBDO& problem,
                                   uint nbHoursInAWeek)
{
    study.areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        RESULTATS_HORAIRES* weeklyResults = problem.ResultatsHoraires[index];

        double* niv = weeklyResults->niveauxHoraires;

        problem.previousSimulationFinalLevel[index]
          = niv[nbHoursInAWeek - 1] * reservoirCapacity / 100;
    });
}

void updatingAnnualFinalHydroLevel(const Data::Study& study, PROBLEME_HEBDO& problem)
{
    if (!problem.hydroHotStart)
        return;

    study.areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        problem.previousYearFinalLevels[index]
          = problem.previousSimulationFinalLevel[index] / reservoirCapacity;
    });
}

} // namespace Simulation
} // namespace Solver
} // namespace Antares
