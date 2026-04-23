// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/storage/minmax-data.h"

#include <algorithm>
#include <limits>

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
static void initArray(bool opInferior, MinMaxData::Data& data)
{
    const double init = opInferior ? std::numeric_limits<double>::max()
                                   : -std::numeric_limits<double>::max();
    std::fill(data.values.begin(), data.values.end(), init);
    std::fill(data.indices.begin(), data.indices.end(), std::numeric_limits<uint16_t>::max());
}

static void mergeArray(bool opInferior, unsigned year, MinMaxData::Data& data, const double* values)
{
    const uint16_t y = year + 1; // The year is zero-based
    for (size_t i = 0; i < data.values.size(); ++i)
    {
        if (opInferior)
        {
            if (values[i] < data.values[i])
            {
                data.values[i] = values[i];
                data.indices[i] = y;
            }
            else if (values[i] == data.values[i] && data.indices[i] > y)
            {
                data.indices[i] = y;
            }
        }
        else
        {
            if (values[i] > data.values[i])
            {
                data.values[i] = values[i];
                data.indices[i] = y;
            }
            else if (values[i] == data.values[i] && data.indices[i] > y)
            {
                data.indices[i] = y;
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
    const double yearArray[1] = {rhs.year};
    mergeArray(true, year, annual, yearArray);
}

void MinMaxData::mergeSup(uint year, const IntermediateValues& rhs)
{
    mergeArray(false, year, monthly, rhs.month);
    mergeArray(false, year, weekly, rhs.week);
    mergeArray(false, year, daily, rhs.day);
    mergeArray(false, year, hourly, rhs.hour);
    const double yearArray[1] = {rhs.year};
    mergeArray(false, year, annual, yearArray);
}

} // namespace Antares::Solver::Variable::R::AllYears
