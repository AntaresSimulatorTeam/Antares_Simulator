// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/storage/averagedata.h"

#include <yuni/yuni.h>

#include "antares/solver/variable/storage/intermediate.h"

using namespace Yuni;

namespace Antares::Solver::Variable::R::AllYears
{
AverageData::AverageData():
    nbYearsCapacity(0),
    allYears(0.)
{
}

AverageData::~AverageData() = default;

void AverageData::reset()
{
    year.assign(nbYearsCapacity, 0.);
    monthly.assign(MONTHS_PER_YEAR, 0.);
    weekly.assign(WEEKS_PER_YEAR, 0.);
    daily.assign(DAYS_PER_YEAR, 0.);
    hourly.assign(HOURS_PER_YEAR, 0.);
}

void AverageData::initializeFromStudy(Data::Study& study)
{
    nbYearsCapacity = study.runtime.rangeLimits.year[Data::rangeEnd] + 1;
    year.resize(nbYearsCapacity);

    yearsWeight = study.parameters.getYearsWeight();
    yearsWeightSum = study.parameters.getYearsWeightSum();
}

void AverageData::merge(unsigned int y, const IntermediateValues& rhs)
{
    unsigned int i;

    // Ratio take into account MC year weight
    double ratio = (double)yearsWeight[y] / (double)yearsWeightSum;

    // Average value for each hour throughout all years
    for (i = 0; i != HOURS_PER_YEAR; ++i)
    {
        hourly[i] += rhs.hour[i] * ratio;
    }
    // Average value for each day throughout all years
    for (i = 0; i != DAYS_PER_YEAR; ++i)
    {
        daily[i] += rhs.day[i] * ratio;
    }
    // Average value for each week throughout all years
    for (i = 0; i != WEEKS_PER_YEAR; ++i)
    {
        weekly[i] += rhs.week[i] * ratio;
    }
    // Average value for each month throughout all years
    for (i = 0; i != MONTHS_PER_YEAR; ++i)
    {
        monthly[i] += rhs.month[i] * ratio;
    }
    // Average value throughout all years
    year[y] += rhs.year * ratio;
}

} // namespace Antares::Solver::Variable::R::AllYears
