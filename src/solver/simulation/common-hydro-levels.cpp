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

#include <antares/study/study.h>
#include "common-eco-adq.h"
#include "simulation.h"
#include <antares/study/parts/hydro/container.h>

namespace Antares::Solver::Simulation
{
using Constants::nbHoursInAWeek;

void computingHydroLevels(const Data::AreaList& areas,
                          PROBLEME_HEBDO& problem,
                          bool remixWasRun,
                          bool computeAnyway)
{
    areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        if (not computeAnyway)
        {
            if (area.hydro.useHeuristicTarget != remixWasRun)
                return;
        }

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        std::vector<double>& inflows
            = problem.CaracteristiquesHydrauliques[index].ApportNaturelHoraire;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];

        std::vector<double>& turb = weeklyResults.TurbinageHoraire;

        std::vector<double>& pump = weeklyResults.PompageHoraire;
        double pumpingRatio = area.hydro.pumpingEfficiency;

        double nivInit = problem.CaracteristiquesHydrauliques[index].NiveauInitialReservoir;
        std::vector<double>& niv = weeklyResults.niveauxHoraires;

        std::vector<double>& ovf = weeklyResults.debordementsHoraires;

        computeTimeStepLevel computeLvlObj(nivInit,
                                           inflows,
                                           ovf,
                                           turb,
                                           pumpingRatio,
                                           pump,
                                           reservoirCapacity);

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

void interpolateWaterValue(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Date::Calendar& calendar,
                           int firstHourOfTheWeek)
{
    uint daysOfWeek[7] = {0, 0, 0, 0, 0, 0, 0};

    const uint weekFirstDay = calendar.hours[firstHourOfTheWeek].dayYear;

    daysOfWeek[0] = weekFirstDay;
    for (int d = 1; d < 7; d++)
        daysOfWeek[d] = weekFirstDay + d;

    areas.each([&](const Data::Area& area) {
        uint index = area.index;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];

        std::vector<double>& waterVal = weeklyResults.valeurH2oHoraire;

        for (uint h = 0; h < nbHoursInAWeek; h++)
            waterVal[h] = 0.;

        if (!area.hydro.reservoirManagement || !area.hydro.useWaterValue)
            return;

        if (!area.hydro.useWaterValue)
            return;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        std::vector<double>& niv = weeklyResults.niveauxHoraires;

        Antares::Data::getWaterValue(
                problem.previousSimulationFinalLevel[index] * 100 / reservoirCapacity,
                area.hydro.waterValues,
                weekFirstDay,
                waterVal[0]);
        for (uint h = 1; h < nbHoursInAWeek; h++)
        {
            Antares::Data::getWaterValue(niv[h - 1],
                                         area.hydro.waterValues,
                                         daysOfWeek[h / 24],
                                         waterVal[h]);
        }
    });
}

void updatingWeeklyFinalHydroLevel(const Data::AreaList& areas,
                                   PROBLEME_HEBDO& problem)
{
    areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];

        std::vector<double>& niv = weeklyResults.niveauxHoraires;

        problem.previousSimulationFinalLevel[index]
          = niv[nbHoursInAWeek - 1] * reservoirCapacity / 100;
    });
}

void updatingAnnualFinalHydroLevel(const Data::AreaList& areas, PROBLEME_HEBDO& problem)
{
    if (!problem.hydroHotStart)
        return;

    areas.each([&](const Data::Area& area) {
        if (!area.hydro.reservoirManagement)
            return;

        uint index = area.index;

        double reservoirCapacity = area.hydro.reservoirCapacity;

        problem.previousYearFinalLevels[index]
          = problem.previousSimulationFinalLevel[index] / reservoirCapacity;
    });
}

} // namespace Antares::Solver::Simulation
