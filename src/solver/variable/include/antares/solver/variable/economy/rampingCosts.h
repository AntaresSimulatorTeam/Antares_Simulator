// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{
struct RampingCostTraits
{
    static std::string Caption()
    {
        return "RAMP COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Ramping Cost throughout all MC years, of all the thermal dispatchable "
               "clusters";
    }

    using ResultsProfile = StandardResults<R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& values,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& state,
                               unsigned int /*numSpace*/)
    {
        if (state.study.parameters.include.thermal_ramping)
        {
            values[state.hourInTheYear] = state
                                            .thermalClusterRampingCostForYear[state.hourInTheYear];
        }
        else
        {
            values[state.hourInTheYear] = 0;
        }
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }
};

using RampingCost = EconomyVariableBase<RampingCostTraits>;

} // namespace Antares::Solver::Variable::Economy
