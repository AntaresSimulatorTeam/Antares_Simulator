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

#include <antares/study/parts/hydro/container.h>
#include <antares/study/study.h>
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/simulation.h"

namespace Antares::Solver::Simulation
{
using Constants::nbHoursInAWeek;

void computingHydroLevels(const Data::AreaList& areas,
                          PROBLEME_HEBDO& problem,
                          bool remixWasRun,
                          bool computeAnyway)
{
    for (const auto& [_, area]: areas)
    {
        if (!area->hydro.reservoirManagement)
        {
            continue;
        }

        if (!computeAnyway && area->hydro.useHeuristicTarget != remixWasRun)
        {
            continue;
        }

        uint index = area->index;

        double reservoirCapacity = area->hydro.reservoirCapacity;

        std::vector<double>& inflows = problem.CaracteristiquesHydrauliques[index]
                                         .ApportNaturelHoraire;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];
        auto& hydroUsage = weeklyResults.HydroUsage;

        double pumpingRatio = area->hydro.pumpingEfficiency;

        double nivInit = problem.CaracteristiquesHydrauliques[index].NiveauInitialReservoir;

        computeTimeStepLevel computeLvlObj(nivInit, inflows, hydroUsage, pumpingRatio, reservoirCapacity);

        for (uint h = 0; h < nbHoursInAWeek - 1; h++)
        {
            computeLvlObj.run();
            weeklyResults.HydroUsage[h].niveauxHoraires = computeLvlObj.getLevel() * 100
                                                          / reservoirCapacity;
            computeLvlObj.prepareNextStep();
        }

        computeLvlObj.run();
        weeklyResults.HydroUsage[nbHoursInAWeek - 1].niveauxHoraires = computeLvlObj.getLevel()
                                                                       * 100 / reservoirCapacity;
    }
}

void interpolateWaterValue(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Date::Calendar& calendar,
                           int firstHourOfTheWeek)
{
    uint daysOfWeek[7] = {0, 0, 0, 0, 0, 0, 0};

    const uint weekFirstDay = calendar.hours[firstHourOfTheWeek].dayYear;

    daysOfWeek[0] = weekFirstDay;
    for (int d = 1; d < 7; d++)
    {
        daysOfWeek[d] = weekFirstDay + d;
    }

    for (const auto& [_, area]: areas)
    {
        uint index = area->index;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];

        for (auto& res : weeklyResults.HydroUsage) {
            res.valeurH2oHoraire = 0;
        }

        if (!area->hydro.reservoirManagement || !area->hydro.useWaterValue)
        {
            return;
        }

        if (!area->hydro.useWaterValue)
        {
            return;
        }

        double reservoirCapacity = area->hydro.reservoirCapacity;

        weeklyResults.HydroUsage[0].valeurH2oHoraire = Data::getWaterValue(
          problem.previousSimulationFinalLevel[index] * 100 / reservoirCapacity,
          area->hydro.waterValues,
          weekFirstDay);

        for (uint h = 1; h < nbHoursInAWeek; h++)
        {
            weeklyResults.HydroUsage[h].valeurH2oHoraire = Data::getWaterValue(
              weeklyResults.HydroUsage[h - 1].niveauxHoraires,
              area->hydro.waterValues,
              daysOfWeek[h / 24]);
        }
    }
}

void updatingWeeklyFinalHydroLevel(const Data::AreaList& areas, PROBLEME_HEBDO& problem)
{
    for (const auto& [_, area]: areas)
    {
        if (!area->hydro.reservoirManagement)
        {
            continue;
        }

        uint index = area->index;

        double reservoirCapacity = area->hydro.reservoirCapacity;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];

        problem.previousSimulationFinalLevel[index] = weeklyResults.HydroUsage[nbHoursInAWeek - 1]
                                                        .niveauxHoraires
                                                      * reservoirCapacity / 100;
    }
}

} // namespace Antares::Solver::Simulation
