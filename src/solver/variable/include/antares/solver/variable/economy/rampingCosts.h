// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

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

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>,
      R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void yearEndBuildForEachThermalCluster(IntermediateValues& yearlyValues,
                                                  const State& state,
                                                  unsigned int /*year*/,
                                                  unsigned int /*numSpace*/)
    {
        // Sum the ramping cost of every thermal cluster of the area over the year.
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            yearlyValues[i] += state.thermalClusterRampingCostForYear[i];
        }
    }

    // The ramping cost is accumulated at the end of the year (see above), not
    // hour by hour, so no hourly value is ever stored.
    static bool checkCondition(const State&)
    {
        return false;
    }

    static double value(const State&)
    {
        return 0.;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardRampingCost = VCard_Base<RampingCostTraits>;

/*!
** \brief Ramping cost expected from all the thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
using RampingCost = Economy_Base<RampingCostTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
