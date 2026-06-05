// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LMRViolationsTraits
{
    static std::string Caption()
    {
        return "LMR VIOL.";
    }

    static std::string Unit()
    {
        return " ";
    }

    static std::string Description()
    {
        return "Local Matching Rule is violated more than the provided threshold";
    }

    using ResultsProfile = Results<std::tuple<R::AllYears::Average>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesLmrViolations[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        iv[state.hourInTheYear] = value(state);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using LMRViolations = EconomyVariableBase<LMRViolationsTraits>;

} // namespace Antares::Solver::Variable::Economy
