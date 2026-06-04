// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Solver::Variable::Economy
{

struct LOLP_Base_Traits
{
    static std::string Unit()
    {
        return "%";
    }

    using ResultsProfile = Results<std::tuple<R::AllYears::Average>>;

    static constexpr uint8_t decimal = 2;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateOr;

    static double value(const State&)
    {
        return 100.;
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsOrForTheCurrentYear();
    }
};

} // namespace Antares::Solver::Variable::Economy
