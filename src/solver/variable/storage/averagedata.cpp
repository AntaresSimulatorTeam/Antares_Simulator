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

#include <yuni/yuni.h>
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/storage/averagedata.h"

using namespace Yuni;

namespace Antares::Solver::Variable::R::AllYears
{
AverageData::AverageData() : hourly(nullptr), year(nullptr), nbYearsCapacity(0), allYears(0.)
{
}

AverageData::~AverageData()
{
    Antares::Memory::Release(hourly);
    delete[] year;
}

void AverageData::reset()
{
    Antares::Memory::Zero(maxHoursInAYear, hourly);
    (void)::memset(monthly, 0, sizeof(double) * maxMonths);
    (void)::memset(weekly, 0, sizeof(double) * maxWeeksInAYear);
    (void)::memset(daily, 0, sizeof(double) * maxDaysInAYear);
    (void)::memset(year, 0, sizeof(double) * nbYearsCapacity);
}

void AverageData::initializeFromStudy(Data::Study& study)
{
    Antares::Memory::Allocate<double>(hourly, maxHoursInAYear);
    nbYearsCapacity = study.runtime->rangeLimits.year[Data::rangeEnd] + 1;
    year = new double[nbYearsCapacity];

    yearsWeight = study.parameters.getYearsWeight();
    yearsWeightSum = study.parameters.getYearsWeightSum();
}

void AverageData::merge(unsigned int y, const IntermediateValues& rhs)
{
    unsigned int i;

    // Ratio take into account MC year weight
    double ratio = (double)yearsWeight[y] / (double)yearsWeightSum;

    // Average value for each hour throughout all years
    for (i = 0; i != maxHoursInAYear; ++i)
        hourly[i] += rhs.hour[i] * ratio;
    // Average value for each day throughout all years
    for (i = 0; i != maxDaysInAYear; ++i)
        daily[i] += rhs.day[i] * ratio;
    // Average value for each week throughout all years
    for (i = 0; i != maxWeeksInAYear; ++i)
        weekly[i] += rhs.week[i] * ratio;
    // Average value for each month throughout all years
    for (i = 0; i != maxMonths; ++i)
        monthly[i] += rhs.month[i] * ratio;
    // Average value throughout all years
    year[y] += rhs.year * ratio;
}

} // namespace Antares::Solver::Variable::R::AllYears




