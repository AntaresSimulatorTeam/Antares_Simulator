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

#include "antares/study/area/scratchpad.h"

#include <limits>

#include "antares/antares/antares.h"
#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data
{

bool doWeHaveOnePositiveMaxDailyEnergy(const Matrix<double>& dailyPower,
                                       const Matrix<double>::ColumnType& nbHoursAtPmaxPerDay)
{
    for (uint tsNumber = 0; tsNumber < dailyPower.width; ++tsNumber)
    {
        for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        {
            if (dailyPower[tsNumber][day] * nbHoursAtPmaxPerDay[day] > 0.)
            {
                return true;
            }
        }
    }

    return false;
}

void CalculateDailyMeanPower(const Matrix<double>::ColumnType& hourlyColumn,
                             Matrix<double>::ColumnType& dailyColumn)
{
    for (uint day = 0; day < DAYS_PER_YEAR; ++day)
    {
        dailyColumn[day] = std::accumulate(hourlyColumn + day * HOURS_PER_DAY,
                                           hourlyColumn + day * HOURS_PER_DAY + HOURS_PER_DAY,
                                           0)
                           / 24.;
    }
}

AreaScratchpad::AreaScratchpad(const StudyRuntimeInfos& rinfos, Area& area):
    meanMaxDailyGenPower(area.hydro.series->timeseriesNumbers),
    meanMaxDailyPumpPower(area.hydro.series->timeseriesNumbers)
{
    // alias to the simulation mode
    auto mode = rinfos.mode;

    for (uint i = 0; i != 168; ++i)
    {
        dispatchableGenerationMargin[i] = 0;
    }

    for (uint h = 0; h != HOURS_PER_YEAR; ++h)
    {
        mustrunSum[h] = std::numeric_limits<double>::quiet_NaN();
        originalMustrunSum[h] = std::numeric_limits<double>::quiet_NaN();
    }

    // Fatal hors hydro
    {
        double sum;
        uint w;
        assert(area.miscGen.height > 0);
        assert(area.miscGen.width > 0);
        uint height = area.miscGen.height;
        for (uint h = 0; h != height; ++h)
        {
            sum = 0.;
            for (w = 0; w != area.miscGen.width; ++w)
            {
                sum += area.miscGen[w][h];
            }
            miscGenSum[h] = sum;
        }
        if (mode == Data::SimulationMode::Adequacy)
        {
            for (uint h = 0; h != area.miscGen.height; ++h)
            {
                miscGenSum[h] -= area.reserves[Data::fhrPrimaryReserve][h];
            }
        }
    }

    //*******************************************************************************
    // TODO : about computing hydro max power daily mean from hourly max power TS.
    //*******************************************************************************
    //   - This computation is done here, but we don't want it here.
    //     We want Scratchpad to shrink and even disappear.
    //     So a possible solution to move this computation to some place else is to host
    //     these means TS in the hydro part of areas, and compute them right after
    //     their the hourly TS (max power).
    //     Note that scratchpad instances are duplicated for multi-threading purpose,
    //     and that moving these TS elsewhere could create concurrency issues.
    //     But these daily TS, once computed, are then only read (in daily.cpp
    //     and when building the weekly optimization problem).
    //     Thus we don't have to fear such issues.
    //   - Besides, there is a performance problem here : for a given area, we compute
    //     the max power daily means for each call to scratchpad constructor, that is
    //     the same computation for each thread.
    //     This is another reason to move the computation from here.
    //*******************************************************************************

    //  Hourly maximum generation/pumping power matrices and their number of TS's (width of
    //  matrices)
    const auto& maxHourlyGenPower = area.hydro.series->maxHourlyGenPower.timeSeries;
    const auto& maxHourlyPumpPower = area.hydro.series->maxHourlyPumpPower.timeSeries;

    //  Setting width and height of daily mean maximum generation/pumping power matrices
    meanMaxDailyGenPower.timeSeries.reset(maxHourlyGenPower.width, DAYS_PER_YEAR);
    meanMaxDailyPumpPower.timeSeries.reset(maxHourlyPumpPower.width, DAYS_PER_YEAR);

    // Instantiate daily mean maximum generation/pumping power matrices
    CalculateMeanDailyMaxPowerMatrices(maxHourlyGenPower, maxHourlyPumpPower);

    // ===============
    // hydroHasMod
    // ===============

    // ------------------------------
    // Hydro generation permission
    // ------------------------------
    // Useful whether we use a heuristic target or not
    bool hydroGenerationPermission = false;

    // ... Getting hydro max energy
    const auto& dailyNbHoursAtGenPmax = area.hydro.dailyNbHoursAtGenPmax[0];

    hydroGenerationPermission = doWeHaveOnePositiveMaxDailyEnergy(meanMaxDailyGenPower.timeSeries,
                                                                  dailyNbHoursAtGenPmax);

    // ---------------------
    // Hydro has inflows
    // ---------------------
    bool hydroHasInflows = false;
    if (!area.hydro.prepro) // not in prepro mode
    {
        assert(area.hydro.series);
        hydroHasInflows = MatrixTestForAtLeastOnePositiveValue(
          area.hydro.series->storage.timeSeries);
    }
    else
    {
        auto& m = area.hydro.prepro->data;
        double valueCol = 0.;
        auto& colPowerOverWater = m[PreproHydro::powerOverWater];
        auto& colMaxEnergy = m[PreproHydro::maximumEnergy];

        for (uint month = 0; month < MONTHS_PER_YEAR; ++month)
        {
            valueCol += colMaxEnergy[month] * (1. - colPowerOverWater[month]);
        }

        hydroHasInflows = (valueCol > 0.);
    }

    // --------------------------
    // hydroHasMod definition
    // --------------------------
    hydroHasMod = hydroHasInflows || hydroGenerationPermission;

    // ===============
    // Pumping
    // ===============

    //  Hydro max pumping energy
    const auto& dailyNbHoursAtPumpPmax = area.hydro.dailyNbHoursAtPumpPmax[0];

    //  If pumping energy is nil over the whole year, pumpHasMod is false, true otherwise.
    pumpHasMod = doWeHaveOnePositiveMaxDailyEnergy(meanMaxDailyPumpPower.timeSeries,
                                                   dailyNbHoursAtPumpPmax);
}

void AreaScratchpad::CalculateMeanDailyMaxPowerMatrices(const Matrix<double>& hourlyMaxGenMatrix,
                                                        const Matrix<double>& hourlyMaxPumpMatrix)
{
    for (uint nbOfTimeSeries = 0; nbOfTimeSeries < hourlyMaxGenMatrix.width; ++nbOfTimeSeries)
    {
        auto& hourlyMaxGenColumn = hourlyMaxGenMatrix[nbOfTimeSeries];
        auto& MeanMaxDailyGenPowerColumn = meanMaxDailyGenPower.timeSeries[nbOfTimeSeries];
        CalculateDailyMeanPower(hourlyMaxGenColumn, MeanMaxDailyGenPowerColumn);
    }

    for (uint nbOfTimeSeries = 0; nbOfTimeSeries < hourlyMaxPumpMatrix.width; ++nbOfTimeSeries)
    {
        auto& MeanMaxDailyPumpPowerColumn = meanMaxDailyPumpPower.timeSeries[nbOfTimeSeries];
        auto& hourlyMaxPumpColumn = hourlyMaxPumpMatrix[nbOfTimeSeries];
        CalculateDailyMeanPower(hourlyMaxPumpColumn, MeanMaxDailyPumpPowerColumn);
    }
}
} // namespace Antares::Data
