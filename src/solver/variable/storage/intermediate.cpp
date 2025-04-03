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

#include "antares/solver/variable/storage/intermediate.h"

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/study/study.h>

using namespace Yuni;

namespace Antares::Solver::Variable
{
IntermediateValues::IntermediateValues():
    pRange(nullptr),
    calendar(nullptr),
    year(0.)
{
    Antares::Memory::Allocate<Type>(hour, HOURS_PER_YEAR);
    Antares::Memory::Zero(HOURS_PER_YEAR, hour);
    (void)::memset(month, 0, sizeof(Type) * MONTHS_PER_YEAR);
    (void)::memset(week, 0, sizeof(Type) * WEEKS_PER_YEAR);
    (void)::memset(day, 0, sizeof(Type) * DAYS_PER_YEAR);
}

void IntermediateValues::initializeFromStudy(Data::Study& study)
{
    pRange = &study.runtime.rangeLimits;
    calendar = &study.calendarOutput;
    pRuntimeInfo = &study.runtime;
}

void IntermediateValues::computeStatisticsAdequacyForTheCurrentYear()
{
    year = 0.;

    // Detecting large buffer overflow
    assert(pRange);
    assert(calendar);
    pRange->checkIntegrity();

    // x(d)
    // For each day in the year

    for (uint i = pRange->hour[Data::rangeBegin]; i <= pRange->hour[Data::rangeEnd]; ++i)
    {
        year += hour[i];
    }
}

void IntermediateValues::computeStatisticsForTheCurrentYear()
{
    uint i;
    uint j;

    year = 0.;

    // Detecting large buffer overflow
    assert(pRange);
    assert(calendar);
    pRange->checkIntegrity();

    // x(d)
    // For each day in the year
    uint indx = pRange->hour[Data::rangeBegin];

    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        double d = 0.;
        // One day
        for (j = 0; j != HOURS_PER_DAY; ++j)
        {
            assert(indx < HOURS_PER_YEAR);
            d += hour[indx];
            ++indx;
        }
        day[i] = d;
        year += d;
    }

    // weeks
    for (i = 0; i != WEEKS_PER_YEAR; ++i)
    {
        week[i] = 0.;
    }
    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        week[calendar->days[i].week] += day[i];
    }

    // x(m)
    indx = calendar->months[pRange->month[Data::rangeBegin]].daysYear.first;
    for (i = pRange->month[Data::rangeBegin]; i <= pRange->month[Data::rangeEnd]; ++i)
    {
        double d = 0.;
        uint daysInMonth = calendar->months[i].days;
        for (j = 0; j != daysInMonth; ++j)
        {
            assert(indx < 7 * 53 + 1);
            d += day[indx];
            ++indx;
        }
        month[i] = d;
    }
}

void IntermediateValues::computeStatisticsOrForTheCurrentYear()
{
    uint i;
    uint j;

    year = 0.;

    // Detecting large buffer overflow
    assert(pRange);
    pRange->checkIntegrity();

    // x(d)
    // For each day in the year
    uint indx = pRange->hour[Data::rangeBegin];

    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        day[i] = 0.;
        // One day
        for (j = 0; j != HOURS_PER_DAY; ++j)
        {
            assert(indx < HOURS_PER_YEAR);
            if (hour[indx] > 0.)
            {
                day[i] = 100.;
            }
            ++indx;
        }
    }

    // weeks
    for (i = 0; i != WEEKS_PER_YEAR; ++i)
    {
        week[i] = 0.;
    }
    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        if (day[i] > 0)
        {
            week[calendar->days[i].week] = 100.;
        }
    }

    // x(m)
    indx = calendar->months[pRange->month[Data::rangeBegin]].daysYear.first;
    for (i = pRange->month[Data::rangeBegin]; i <= pRange->month[Data::rangeEnd]; ++i)
    {
        uint daysInMonth = calendar->months[i].days;
        for (j = 0; j != daysInMonth; ++j)
        {
            assert(indx < 7 * 53);
            if (day[indx] > 0.)
            {
                month[i] = 100;
                year = 100.;
            }
            ++indx;
        }
    }
}

void IntermediateValues::computeAveragesForCurrentYearFromHourlyResults()
{
    // Detecting large buffer overflow
    assert(pRange);
    pRange->checkIntegrity();

    // Form hourly results of current year, compute average results
    // for other time divisions of the current year : days of the year, weeks of the year, ...
    computeDailyAveragesForCurrentYear();
    computeWeeklyAveragesForCurrentYear();
    computeMonthlyAveragesForCurrentYear();
    computeYearlyAveragesForCurrentYear();
}

void IntermediateValues::computeAveragesForCurrentYearFromDailyResults()
{
    computeWeeklyAveragesForCurrentYear();
    computeMonthlyAveragesForCurrentYear();
    computeYearlyAveragesForCurrentYear();
}

void IntermediateValues::computeAveragesForCurrentYearFromWeeklyResults()
{
    computeMonthlyAveragesForCurrentYear();
    computeYearlyAveragesForCurrentYear();
}

void IntermediateValues::computeDailyAveragesForCurrentYear()
{
    // Compute days average for each day of the year
    double day_sum;
    uint indx = pRange->hour[Data::rangeBegin];
    for (uint d = pRange->day[Data::rangeBegin]; d <= pRange->day[Data::rangeEnd]; ++d)
    {
        // Compute sum of hourly values on the current day of year
        day_sum = 0.;
        for (uint h = 0; h != HOURS_PER_DAY; ++h)
        {
            day_sum += hour[indx];
            ++indx;
        }
        day[d] = day_sum / HOURS_PER_DAY;
    }
}

void IntermediateValues::computeWeeklyAveragesForCurrentYear()
{
    // Re-initialization (a previous MC year could have left non-nil values)
    for (int w = 0; w != WEEKS_PER_YEAR; ++w)
    {
        week[w] = 0.;
    }

    // Compute weekly averages for each week in the year
    for (uint d = pRange->day[Data::rangeBegin]; d <= pRange->day[Data::rangeEnd]; ++d)
    {
        week[calendar->days[d].week] += day[d];
    }
    for (uint w = pRange->week[Data::rangeBegin]; w <= pRange->week[Data::rangeEnd]; ++w)
    {
        week[w] /= pRuntimeInfo->simulationDaysPerWeek[w];
    }
}

void IntermediateValues::computeMonthlyAveragesForCurrentYear()
{
    // Compute monthly averages for each month in the year :
    //    We need daily values in order to compute monthly averages.
    //    Indeed, weekly values would be suitable for this : there are not necessarily an
    //    integer number of weeks in a month.
    double month_sum;
    uint indx = calendar->months[pRange->month[Data::rangeBegin]].daysYear.first;
    for (uint m = pRange->month[Data::rangeBegin]; m <= pRange->month[Data::rangeEnd]; ++m)
    {
        month_sum = 0.;
        uint daysInMonth = calendar->months[m].days;
        for (uint d = 0; d != daysInMonth; ++d)
        {
            assert(indx < 7 * 53 + 1);
            month_sum += day[indx];
            ++indx;
        }
        month[m] = month_sum / pRuntimeInfo->simulationDaysPerMonth[m];
    }
}

void IntermediateValues::computeYearlyAveragesForCurrentYear()
{
    year = 0.; // Re-initialization (a previous MC year could have left a non-nil value)
    for (uint d = pRange->day[Data::rangeBegin]; d <= pRange->day[Data::rangeEnd]; ++d)
    {
        year += day[d];
    }
    year *= HOURS_PER_DAY / (double)pRange->hour[Data::rangeCount];
}

void IntermediateValues::computeProbabilitiesForTheCurrentYear()
{
    uint i;
    uint j;
    double d;

    // Detecting large buffer overflow
    assert(pRange);
    pRange->checkIntegrity();

    double ratio = 100. / pRange->year[Data::rangeCount];

    year = 0.;

    // x(d)
    // For each day in the year
    uint indx = pRange->hour[Data::rangeBegin];
    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        d = 0.;
        // One day
        for (j = 0; j != HOURS_PER_DAY; ++j)
        {
            if (hour[indx] > 0.)
            {
                d = ratio;
            }
            ++indx;
        }
        day[i] = d;
    }

    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        if (day[i] > 0.)
        {
            year = ratio;
            break;
        }
    }

    // weeks
    for (i = 0; i != WEEKS_PER_YEAR; ++i)
    {
        week[i] = 0.;
    }
    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        if (day[i] > 0.)
        {
            uint weekindex = calendar->days[i].week;
            week[weekindex] = ratio; //+= ratio / calendar->weeks[weekindex].days;
        }
    }

    // x(m)
    indx = calendar->months[pRange->month[Data::rangeBegin]].daysYear.first;
    for (i = pRange->month[Data::rangeBegin]; i <= pRange->month[Data::rangeEnd]; ++i)
    {
        d = 0.;
        uint daysInMonth = calendar->months[i].days;
        for (j = 0; j != daysInMonth; ++j)
        {
            assert(indx < 7 * 53 + 1);
            if (day[indx] > 0.)
            {
                d = ratio;
            }
            ++indx;
        }
        month[i] = d;
    }
}

void IntermediateValues::adjustValuesWhenRelatedToAPrice()
{
    uint i;
    double ratio;

    // Months
    ratio = 1. / pRange->month[Data::rangeCount];
    for (i = pRange->month[Data::rangeBegin]; i <= pRange->month[Data::rangeEnd]; ++i)
    {
        month[i] *= ratio;
    }

    // Weeks
    ratio = 1. / pRange->week[Data::rangeCount];
    for (i = pRange->week[Data::rangeBegin]; i <= pRange->week[Data::rangeEnd]; ++i)
    {
        week[i] *= ratio;
    }

    // Days
    ratio = 1. / pRange->day[Data::rangeCount];
    for (i = pRange->day[Data::rangeBegin]; i <= pRange->day[Data::rangeEnd]; ++i)
    {
        day[i] *= ratio;
    }

    // Year
    year /= pRange->hour[Data::rangeCount];
}

void IntermediateValues::adjustValuesAdequacyWhenRelatedToAPrice()
{
    // Year
    year /= pRange->hour[Data::rangeCount];
}

} // namespace Antares::Solver::Variable
