/*
** Copyright 2007-2023 RTE
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

#include "scratchpad.h"
#include "constants.h"
#include "../study.h"
#include <limits>

using namespace Yuni;

namespace Antares::Data
{
AreaScratchpad::TimeseriesData::TimeseriesData(Area& area) :
        load(area.load.series->timeSeries), solar(area.solar.series->timeSeries), wind(area.wind.series->timeSeries)
{
}

AreaScratchpad::AreaScratchpad(const StudyRuntimeInfos& rinfos, Area& area) : ts(area)
{
    // alias to the simulation mode
    auto mode = rinfos.mode;
    uint nbMonthsPerYear = 12;

    for (uint i = 0; i != 168; ++i)
        dispatchableGenerationMargin[i] = 0;

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
                sum += area.miscGen[w][h];
            miscGenSum[h] = sum;
        }
        if (mode == Data::stdmAdequacy)
        {
            for (uint h = 0; h != area.miscGen.height; ++h)
                miscGenSum[h] -= area.reserves[Data::fhrPrimaryReserve][h];
        }
    }

    //  Hourly maximum generation/pumping power matrices and their number of TS's (width of matrices)
    auto const& maxHourlyGenPower = area.hydro.series->maxHourlyGenPower;
    auto const& maxHourlyPumpPower = area.hydro.series->maxHourlyPumpPower;
    uint nbOfMaxPowerTimeSeries = area.hydro.series->maxPowerTScount();

    //  Setting width and height of daily mean maximum generation/pumping power matrices
    meanMaxDailyGenPower.reset(nbOfMaxPowerTimeSeries, DAYS_PER_YEAR);
    meanMaxDailyPumpPower.reset(nbOfMaxPowerTimeSeries, DAYS_PER_YEAR);

    // Instantiate daily mean maximum generation/pumping power matrices
    CalculateMeanDailyMaxPowerMatrices(maxHourlyGenPower, maxHourlyPumpPower, nbOfMaxPowerTimeSeries);

    // ===============
    // hydroHasMod
    // ===============

    // ------------------------------
    // Hydro generation permission
    // ------------------------------
    // Useful whether we use a heuristic target or not
    bool hydroGenerationPermission = false;

    // ... Getting hydro max energy
    auto const& maxDailyGenEnergy = area.hydro.maxDailyGenEnergy[0];

    hydroGenerationPermission = CheckForPositiveEnergy(maxHourlyGenPower, maxDailyGenEnergy);

    // ---------------------
    // Hydro has inflows
    // ---------------------
    bool hydroHasInflows = false;
    if (!area.hydro.prepro) // not in prepro mode
    {
        assert(area.hydro.series);
        hydroHasInflows = MatrixTestForAtLeastOnePositiveValue(area.hydro.series->storage);
    }
    else
    {
        auto& m = area.hydro.prepro->data;
        double valueCol = 0.;
        auto& colPowerOverWater = m[PreproHydro::powerOverWater];
        auto& colMaxEnergy = m[PreproHydro::maximumEnergy];

        for (uint month = 0; month < nbMonthsPerYear; ++month)
            valueCol += colMaxEnergy[month] * (1. - colPowerOverWater[month]);

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
    auto const& maxDailyPumpEnergy = area.hydro.maxDailyPumpEnergy[0];

    //  If pumping energy is nil over the whole year, pumpHasMod is false, true otherwise.
    pumpHasMod = CheckForPositiveEnergy(maxHourlyPumpPower, maxDailyPumpEnergy);
}

AreaScratchpad::~AreaScratchpad() = default;

void AreaScratchpad::CalculateMeanDailyMaxPowerMatrices(const Matrix<double>& hourlyMaxGenMatrix,
                                                        const Matrix<double>& hourlyMaxPumpMatrix,
                                                        uint nbOfMaxPowerTimeSeries)
{
    for (uint nbOfTimeSeries = 0; nbOfTimeSeries < nbOfMaxPowerTimeSeries; ++nbOfTimeSeries)
    {
        auto& hourlyMaxGenColumn = hourlyMaxGenMatrix[nbOfTimeSeries];
        auto& hourlyMaxPumpColumn = hourlyMaxPumpMatrix[nbOfTimeSeries];
        auto& MeanMaxDailyGenPowerColumn = meanMaxDailyGenPower[nbOfTimeSeries];
        auto& MeanMaxDailyPumpPowerColumn = meanMaxDailyPumpPower[nbOfTimeSeries];

        CalculateDailyMeanPower(hourlyMaxGenColumn, MeanMaxDailyGenPowerColumn);
        CalculateDailyMeanPower(hourlyMaxPumpColumn, MeanMaxDailyPumpPowerColumn);
    }
}

bool CheckForPositiveEnergy(const Matrix<double, int32_t>& power,
                            const Matrix<double>::ColumnType& energy)
{
    for (uint tsNumber = 0; tsNumber < power.width; ++tsNumber)
    {
        double yearlyMaxGenEnergy = 0;

        for (uint day = 0; day < DAYS_PER_YEAR; ++day)
        {
            yearlyMaxGenEnergy += power[tsNumber][day] * energy[day];

            if (yearlyMaxGenEnergy > 0.)
                return true;
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
                           / HOURS_PER_DAY;
    }
}

} // namespace Antares::Data
