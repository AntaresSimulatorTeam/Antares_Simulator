// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/parts/hydro/container.h>
#include <antares/study/study.h>
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/hydro/management/management.h"

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

std::vector<std::vector<double>> precomputeWeeklyInitialHydroLevels(
    const Data::AreaList& areas,
    const PROBLEME_HEBDO& problem,
    const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
    const Date::Calendar& calendar,
    uint nbWeeks,
    int startHourInYear)
{
    const uint nAreas = areas.size();
    // result[w][a] = initial reservoir level for area a at start of week w
    std::vector<std::vector<double>> result(nbWeeks, std::vector<double>(nAreas, 0.0));

    for (const auto& [_, area]: areas)
    {
        const uint aIdx = area->index;

        if (!area->hydro.reservoirManagement)
        {
            // No reservoir management: level is not meaningful, fill with 0
            for (uint w = 0; w < nbWeeks; ++w)
            {
                result[w][aIdx] = 0.0;
            }
            continue;
        }

        // For areas with TurbinageEntreBornes = false, the initial level for
        // every week is read from hydroVentilationResults.NiveauxReservoirsDebutJours,
        // exactly as SIM_RenseignementProblemeHebdo does.  We replicate that logic.
        //
        // For areas with TurbinageEntreBornes = true, the level should carry
        // forward from the LP solution of the previous week.  As an approximation,
        // we use the ventilation-provided end-of-day levels (same source).
        // This introduces a bounded approximation error acceptable in Fast Mode UC.

        int hourInYear = startHourInYear;
        for (uint w = 0; w < nbWeeks; ++w)
        {
            const uint weekFirstDay = calendar.hours[hourInYear].dayYear;

            const bool turbEntreBornes
              = problem.CaracteristiquesHydrauliques[aIdx].TurbinageEntreBornes;

            if (!turbEntreBornes)
            {
                // Level comes from ventilation results (identical to SIM_RenseignementProblemeHebdo)
                result[w][aIdx] = hydroVentilationResults[aIdx].NiveauxReservoirsDebutJours[weekFirstDay]
                                  * area->hydro.reservoirCapacity;
            }
            else
            {
                if (w == 0)
                {
                    // Year's initial level, set by SetInitialHydroLevel before year() is called
                    result[w][aIdx] = problem.previousSimulationFinalLevel[aIdx];
                }
                else
                {
                    // Approximate: use the ventilation end-of-previous-week level
                    const uint prevWeekLastDay = weekFirstDay - 1;
                    result[w][aIdx]
                      = hydroVentilationResults[aIdx].NiveauxReservoirsFinJours[prevWeekLastDay]
                        * area->hydro.reservoirCapacity;
                }
            }

            hourInYear += static_cast<int>(nbHoursInAWeek);
        }
    }

    return result;
}

} // namespace Antares::Solver::Simulation
