// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
#define __SOLVER_VARIABLE_ECONOMY_OverallCost_H__

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct OverallCostTraits
{
    static std::string Caption()
    {
        return "OV. COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Overall Cost throughout all MC years";
    }

    using ResultsProfile = Results<std::tuple<R::AllYears::Average>, R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static double calculateEnergyDeficitCost(const State& state)
    {
        const auto& hourlyResults = *state.hourlyResults;
        const auto hourInWeek = state.hourInTheWeek;
        const auto& thermal = state.area->thermal;

        const double unsuppliedCost = hourlyResults.ValeursHorairesDeDefaillancePositive[hourInWeek]
                                      * thermal.unsuppliedEnergyCost;

        const double spilledCost = hourlyResults.ValeursHorairesDeDefaillanceNegative[hourInWeek]
                                   * thermal.spilledEnergyCost;

        return unsuppliedCost + spilledCost;
    }

    static double calculateReservesCost(const State& state)
    {
        if (!state.study.parameters.include.reserves)
        {
            return 0.0;
        }

        const auto& reserves = state.problemeHebdo->allReserves.value()[state.area->index];
        const auto& hourlyReserves = state.hourlyResults->Reserves.value()[state.hourInTheWeek];

        double totalReservesCost = 0.0;

        for (const auto& reserve: reserves.areaCapacityReservations)
        {
            const double unsatisfiedCost = hourlyReserves.ValeursHorairesInternalUnsatisfied
                                             [reserve.areaReserveIndex]
                                           * reserve.unsuppliedCost;

            const double excessCost = hourlyReserves.ValeursHorairesInternalExcessReserve
                                        [reserve.areaReserveIndex]
                                      * reserve.spillageCost;

            totalReservesCost += unsatisfiedCost + excessCost;
        }

        return totalReservesCost;
    }

    static double getReserveParticipationCost(const State& state)
    {
        return state.reserveData ? state.reserveData.value()
                                     .at(state.area->index)
                                     .reserveParticipationCostForYear[state.hourInTheYear]
                                 : 0.0;
    }

    template<class Aux>
    static void setHourlyValue(IntermediateValues& iv, Aux&, const State& state, unsigned int)
    {
        const double totalCost = calculateEnergyDeficitCost(state) + calculateReservesCost(state)
                                 + getReserveParticipationCost(state);

        iv[state.hourInTheYear] += totalCost;
        // state is logically non-const; annualSystemCost is a side-channel accumulator.
        const_cast<State&>(state).annualSystemCost += totalCost;
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
            state.annualSystemCost += state.thermalClusterOperatingCostForYear[i];
        }
    }
};

using VCardOverallCost = EconomyVariableCard<OverallCostTraits>;

/*!
** \brief Overall cost over all MC years
*/
using OverallCost = EconomyVariableBase<OverallCostTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_OverallCost_H__
