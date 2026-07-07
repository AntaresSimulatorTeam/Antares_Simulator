// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct OperatingCostTraits
{
    static std::string Caption()
    {
        return "OP. COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Operating Cost throughout all MC years, of all the thermal dispatchable clusters";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    // Hourly contribution: reserve participation cost (when enabled).
    // Thermal cluster operating cost is added at year end below.
    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& values,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& state,
                               unsigned int /*numSpace*/)
    {
        if (state.reserveData)
        {
            values[state.hourInTheYear] += state.reserveData.value()
                                             .at(state.area->index)
                                             .reserveParticipationCostForYear[state.hourInTheYear];
        }
    }

    static void yearEndBuildForEachThermalCluster(IntermediateValues& values,
                                                  State& state,
                                                  uint /*year*/,
                                                  unsigned int /*numSpace*/)
    {
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            values[i] += state.thermalClusterOperatingCostForYear[i];
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using OperatingCost = EconomyVariableBase<OperatingCostTraits>;

} // namespace Antares::Solver::Variable::Economy
