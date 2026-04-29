// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/parts/hydro/container.h>
#include <antares/study/study.h>
#include "antares/solver/simulation/common-eco-adq.h"

namespace Antares::Solver::Simulation
{
using Constants::nbHoursInAWeek;

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

        auto& waterVal = weeklyResults.valeurH2oHoraire;
        std::fill(waterVal.begin(), waterVal.end(), 0.);

        if (!area->hydro.reservoirManagement || !area->hydro.useWaterValue)
        {
            return;
        }

        double reservoirCapacity = area->hydro.reservoirCapacity;

        const std::vector<double>& niv = weeklyResults.niveauxHoraires;

        waterVal[0] = Data::getWaterValue(100 * problem.previousSimulationFinalLevel[index]
                                            / reservoirCapacity,
                                          area->hydro.waterValues,
                                          weekFirstDay);

        for (uint h = 1; h < nbHoursInAWeek; h++)
        {
            waterVal[h] = Data::getWaterValue(100 * niv[h - 1] / reservoirCapacity,
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
        const RESULTATS_HORAIRES& weeklyResults = problem.ResultatsHoraires[index];
        const std::vector<double>& niv = weeklyResults.niveauxHoraires;

        problem.previousSimulationFinalLevel[index] = niv[nbHoursInAWeek - 1];
    }
}

} // namespace Antares::Solver::Simulation
