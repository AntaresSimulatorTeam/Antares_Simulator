// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__
#define __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__

#include <antares/logs/logs.h>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct NbOfDispatchedUnitsTraits
{
    static std::string Caption()
    {
        return "NODU";
    }

    static std::string Unit()
    {
        return " ";
    }

    static std::string Description()
    {
        return "Number Of Dispatched Units throughout all MC years, of all the thermal "
               "dispatchable clusters";
    }

    using ResultsProfile = StandardResults<R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    // This variable produces no hourly value: its results are accumulated at
    // year end in yearEndBuildForEachThermalCluster. We still implement an
    // explicit no-op setHourlyValue so the economy_base contract is satisfied
    // intentionally (rather than by a silent fallback), and we log it once.
    template<class Aux>
    static void setHourlyValue(IntermediateValues&, Aux&, const State&, unsigned int)
    {
        [[maybe_unused]] static const bool logged = []
        {
            Antares::logs.info() << "Variable '" << Caption()
                                 << "' has no hourly value (computed at year end)";
            return true;
        }();
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
            values[i] += state.thermalClusterDispatchedUnitsCountForYear[i];
        }
    }
};

/*!
** \brief Number of dispatched units over all thermal clusters
*/
using NbOfDispatchedUnits = EconomyVariableBase<NbOfDispatchedUnitsTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_NbOfDispatchedUnits_H__
