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

#include "antares/solver/variable/storage/minmax-data.h"

#include <cfloat>

#include <yuni/yuni.h>

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
namespace // anonymous
{
constexpr double eps = 1.e-7;

static void initArray(bool opInferior, std::vector<MinMaxData::Data>& array)
{
    for (auto& data : array)
    {
        data.value = opInferior ? DBL_MAX : -DBL_MAX; // +inf or -inf
        data.indice = (uint32_t)(-1); // invalid indice
    }
}


template<bool OpInferior, uint Size>
struct MergeArray
{
    template<class U>
    static void Do(const uint year, std::vector<MinMaxData::Data>& results, const U& values)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = results[i];

            if (OpInferior)
            {
                if (values[i] < data.value - eps)
                {
                    data.value = values[i];
                    data.indice = year + 1; // The year is zero-based
                }
            }
            else
            {
                if (values[i] > data.value + eps)
                {
                    data.value = values[i];
                    data.indice = year + 1; // The year is zero-based
                }
            }
        }
    }

}; // class MergeArray

} // anonymous namespace

void MinMaxData::resetInf()
{
    initArray(true, annual);
    initArray(true, monthly);
    initArray(true, weekly);
    initArray(true, daily);
    initArray(true, hourly);
}

void MinMaxData::resetSup()
{
    initArray(false, annual);
    initArray(false, monthly);
    initArray(false, weekly);
    initArray(false, daily);
    initArray(false, hourly);
}

void MinMaxData::mergeInf(uint year, const IntermediateValues& rhs)
{
    MergeArray<true, maxMonths>::Do(year, monthly, rhs.month);
    MergeArray<true, maxWeeksInAYear>::Do(year, weekly, rhs.week);
    MergeArray<true, maxDaysInAYear>::Do(year, daily, rhs.day);
    MergeArray<true, maxHoursInAYear>::Do(year, hourly, rhs.hour);
    MergeArray<true, 1>::Do(year, annual, &rhs.year);
}

void MinMaxData::mergeSup(uint year, const IntermediateValues& rhs)
{
    MergeArray<false, maxMonths>::Do(year, monthly, rhs.month);
    MergeArray<false, maxWeeksInAYear>::Do(year, weekly, rhs.week);
    MergeArray<false, maxDaysInAYear>::Do(year, daily, rhs.day);
    MergeArray<false, maxHoursInAYear>::Do(year, hourly, rhs.hour);
    MergeArray<false, 1>::Do(year, annual, &rhs.year);
}

} // namespace Antares::Solver::Variable::R::AllYears
