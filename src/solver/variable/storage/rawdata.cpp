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

#include "antares/solver/variable/storage/rawdata.h"

#include <yuni/yuni.h>

using namespace Yuni;

namespace Antares::Solver::Variable::R::AllYears
{
RawData::RawData():
    hourly(nullptr),
    year(nullptr),
    allYears(0.)
{
}

RawData::~RawData()
{
    Antares::Memory::Release(hourly);
    delete[] year;
}

void RawData::initializeFromStudy(const Data::Study& study)
{
    Antares::Memory::Allocate<double>(hourly, maxHoursInAYear);
    nbYearsCapacity = study.runtime->rangeLimits.year[Data::rangeEnd] + 1;
    year = new double[nbYearsCapacity];
}

void RawData::reset()
{
    // Reset
    Antares::Memory::Zero(maxHoursInAYear, hourly);
    (void)::memset(monthly, 0, sizeof(double) * MONTHS_PER_YEAR);
    (void)::memset(weekly, 0, sizeof(double) * WEEKS_PER_YEAR);
    (void)::memset(daily, 0, sizeof(double) * DAYS_PER_YEAR);
    (void)::memset(year, 0, sizeof(double) * nbYearsCapacity);
}

void RawData::merge(unsigned int y, const IntermediateValues& rhs)
{
    unsigned int i;
    // StdDeviation value for each hour throughout all years
    for (i = 0; i != maxHoursInAYear; ++i)
    {
        hourly[i] += rhs.hour[i];
    }
    // StdDeviation value for each day throughout all years
    for (i = 0; i != DAYS_PER_YEAR; ++i)
    {
        daily[i] += rhs.day[i];
    }
    // StdDeviation value for each week throughout all years
    for (i = 0; i != WEEKS_PER_YEAR; ++i)
    {
        weekly[i] += rhs.week[i];
    }
    // StdDeviation value for each month throughout all years
    for (i = 0; i != MONTHS_PER_YEAR; ++i)
    {
        monthly[i] += rhs.month[i];
    }
    // StdDeviation value throughout all years
    year[y] += rhs.year;
}

} // namespace Antares::Solver::Variable::R::AllYears
