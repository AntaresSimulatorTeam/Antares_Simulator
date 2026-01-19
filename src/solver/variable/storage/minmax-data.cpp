// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/storage/minmax-data.h"

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
static void initArray(bool opInferior, std::vector<MinMaxData::Data>& array)
{
    for (auto& data: array)
    {
        data.value = opInferior ? DBL_MAX : -DBL_MAX; // +inf or -inf
        data.index = static_cast<uint32_t>(-1);
    }
}

static void mergeArray(bool opInferior,
                       unsigned year,
                       std::vector<MinMaxData::Data>& results,
                       const double* values)
{
    for (unsigned i = 0; i < results.size(); ++i)
    {
        MinMaxData::Data& data = results[i];

        if (opInferior)
        {
            if (values[i] < data.value)
            {
                data.value = values[i];
                data.index = year + 1;
            }
            else if (values[i] == data.value && data.index > year + 1)
            {
                data.index = year + 1;
            }
        }
        else
        {
            if (values[i] > data.value)
            {
                data.value = values[i];
                data.index = year + 1; // The year is zero-based
            }
            else if (values[i] == data.value && data.index > year + 1)
            {
                data.index = year + 1;
            }
        }
    }
}

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
    mergeArray(true, year, monthly, rhs.month);
    mergeArray(true, year, weekly, rhs.week);
    mergeArray(true, year, daily, rhs.day);
    mergeArray(true, year, hourly, rhs.hour);
    mergeArray(true, year, annual, &rhs.year);
}

void MinMaxData::mergeSup(uint year, const IntermediateValues& rhs)
{
    mergeArray(false, year, monthly, rhs.month);
    mergeArray(false, year, weekly, rhs.week);
    mergeArray(false, year, daily, rhs.day);
    mergeArray(false, year, hourly, rhs.hour);
    mergeArray(false, year, annual, &rhs.year);
}

} // namespace Antares::Solver::Variable::R::AllYears
