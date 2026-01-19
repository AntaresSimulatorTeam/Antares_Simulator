// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/storage/rawdata.h"

#include <yuni/yuni.h>

using namespace Yuni;

namespace Antares::Solver::Variable::R::AllYears
{
RawData::RawData():
    hourly(nullptr),
    allYears(0.)
{
}

RawData::~RawData()
{
    Antares::Memory::Release(hourly);
}

void RawData::initializeFromStudy(const Data::Study& study)
{
    Antares::Memory::Allocate<double>(hourly, HOURS_PER_YEAR);
    nbYearsCapacity = study.runtime.rangeLimits.year[Data::rangeEnd] + 1;
    year.resize(nbYearsCapacity);
}

void RawData::reset()
{
    // Reset
    Antares::Memory::Zero(HOURS_PER_YEAR, hourly);
    (void)::memset(monthly, 0, sizeof(double) * MONTHS_PER_YEAR);
    (void)::memset(weekly, 0, sizeof(double) * WEEKS_PER_YEAR);
    (void)::memset(daily, 0, sizeof(double) * DAYS_PER_YEAR);
    year.assign(nbYearsCapacity, 0);
}

void RawData::merge(unsigned int y, const IntermediateValues& rhs)
{
    unsigned int i;
    // StdDeviation value for each hour throughout all years
    for (i = 0; i != HOURS_PER_YEAR; ++i)
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
