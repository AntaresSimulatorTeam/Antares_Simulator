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

#include <yuni/yuni.h>
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/storage/minmax-data.h"
#include <cfloat>

using namespace Yuni;

namespace Antares::Solver::Variable::R::AllYears
{
namespace // anonymous
{
constexpr double eps = 1.e-7;

template<uint Size, bool OpInferior>
struct ArrayInitializer
{
    static void Init(Antares::Memory::Array<MinMaxData::Data>& array)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = array[i];
            data.value = DBL_MAX;         // +inf
            data.indice = (uint32_t)(-1); // invalid indice
        }
    }

    static void Init(MinMaxData::Data* array)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = array[i];
            data.value = DBL_MAX;         // +inf
            data.indice = (uint32_t)(-1); // invalid indice
        }
    }

}; // class ArrayInitializer

template<uint Size>
struct ArrayInitializer<Size, false>
{
    static void Init(Antares::Memory::Array<MinMaxData::Data>& array)
    {
        for (uint i = 0; i != Size; ++i)
        {
            // Contrary to what we could guess, DBL_MIN is not the smallest number
            // you can hold in a double, but the smallest positive number you can
            // hold in a double
            MinMaxData::Data& data = array[i];
            data.value = -DBL_MAX;        // -inf
            data.indice = (uint32_t)(-1); // invalid indice
        }
    }

    static void Init(MinMaxData::Data* array)
    {
        for (uint i = 0; i != Size; ++i)
        {
            // Contrary to what we could guess, DBL_MIN is not the smallest number
            // you can hold in a double, but the smallest positive number you can
            // hold in a double
            MinMaxData::Data& data = array[i];
            data.value = -DBL_MAX;        // -inf
            data.indice = (uint32_t)(-1); // invalid indice
        }
    }

}; // class ArrayInitializer

template<bool OpInferior, uint Size>
struct MergeArray
{
    template<class U>
    static void Do(const uint year,
                   Antares::Memory::Array<MinMaxData::Data>& results,
                   const U& values)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = results[i];
            if (values[i] < data.value - eps)
            {
                data.value = values[i];
                data.indice = year + 1; // The year is zero-based
            }
        }
    }

    template<class U>
    static void Do(const uint year, MinMaxData::Data* results, const U& values)
    {
        for (uint i = 0; i != Size; ++i)
        {
            if (values[i] < results[i].value - eps)
            {
                results[i].value = values[i];
                results[i].indice = year + 1; // The year is zero-based
            }
        }
    }

}; // class MergeArray

template<uint Size>
struct MergeArray<0, Size>
{
    template<class U>
    static void Do(const uint year,
                   Antares::Memory::Array<MinMaxData::Data>& results,
                   const U& values)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = results[i];
            if (values[i] > data.value + eps)
            {
                data.value = values[i];
                data.indice = year + 1; // The year is zero-based
            }
        }
    }

    template<class U>
    static void Do(const uint year, MinMaxData::Data* results, const U& values)
    {
        for (uint i = 0; i != Size; ++i)
        {
            MinMaxData::Data& data = results[i];
            if (values[i] > data.value + eps)
            {
                data.value = values[i];
                data.indice = year + 1; // The year is zero-based
            }
        }
    }

}; // class MergeArray

} // anonymous namespace

MinMaxData::MinMaxData():
    hourly(nullptr)
{
}

MinMaxData::~MinMaxData()
{
    Antares::Memory::Release(hourly);
}

void MinMaxData::resetInf()
{
    ArrayInitializer<1, true>::Init(&annual);
    ArrayInitializer<maxMonths, true>::Init(monthly);
    ArrayInitializer<maxWeeksInAYear, true>::Init(weekly);
    ArrayInitializer<maxDaysInAYear, true>::Init(daily);
    ArrayInitializer<maxHoursInAYear, true>::Init(hourly);
}

void MinMaxData::resetSup()
{
    ArrayInitializer<1, false>::Init(&annual);
    ArrayInitializer<maxMonths, false>::Init(monthly);
    ArrayInitializer<maxWeeksInAYear, false>::Init(weekly);
    ArrayInitializer<maxDaysInAYear, false>::Init(daily);
    ArrayInitializer<maxHoursInAYear, false>::Init(hourly);
}

void MinMaxData::initialize()
{
    Antares::Memory::Allocate(hourly, maxHoursInAYear);
}

void MinMaxData::mergeInf(uint year, const IntermediateValues& rhs)
{
    MergeArray<true, maxMonths>::Do(year, monthly, rhs.month);
    MergeArray<true, maxWeeksInAYear>::Do(year, weekly, rhs.week);
    MergeArray<true, maxDaysInAYear>::Do(year, daily, rhs.day);
    MergeArray<true, maxHoursInAYear>::Do(year, hourly, rhs.hour);
    MergeArray<true, 1>::Do(year, &annual, &rhs.year);
}

void MinMaxData::mergeSup(uint year, const IntermediateValues& rhs)
{
    MergeArray<false, maxMonths>::Do(year, monthly, rhs.month);
    MergeArray<false, maxWeeksInAYear>::Do(year, weekly, rhs.week);
    MergeArray<false, maxDaysInAYear>::Do(year, daily, rhs.day);
    MergeArray<false, maxHoursInAYear>::Do(year, hourly, rhs.hour);
    MergeArray<false, 1>::Do(year, &annual, &rhs.year);
}

} // namespace Antares::Solver::Variable::R::AllYears
