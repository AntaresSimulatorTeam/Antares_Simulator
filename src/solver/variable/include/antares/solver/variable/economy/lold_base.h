// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Solver::Variable::Economy
{

struct LOLD_Base_Traits
{
    static std::string Unit()
    {
        return "Hours";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 4;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSumThen1IfPositive;

    static double value(const State&)
    {
        return 1.;
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }
};

} // namespace Antares::Solver::Variable::Economy
