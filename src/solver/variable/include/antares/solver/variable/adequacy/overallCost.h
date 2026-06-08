// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__
#define __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__

#include "antares/solver/variable/economy/economy_base.h"

namespace Antares::Solver::Variable
{
namespace Adequacy
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

    static double calculateEnergyDeficitCosts(const State& state)
    {
        const auto hourInWeek = state.hourInTheWeek;
        const auto& hourlyResults = *state.hourlyResults;

        const double unsuppliedCost = hourlyResults.ValeursHorairesDeDefaillancePositive[hourInWeek]
                                      * state.area->thermal.unsuppliedEnergyCost;

        const double spilledCost = (hourlyResults.ValeursHorairesDeDefaillanceNegative[hourInWeek]
                                    + state.resSpilled.entry[state.area->index][hourInWeek])
                                   * state.area->thermal.spilledEnergyCost;

        return unsuppliedCost + spilledCost;
    }

    static double calculateHydroCosts(const State& state)
    {
        const auto hourInWeek = state.hourInTheWeek;
        const auto& hourlyResults = *state.hourlyResults;
        const auto& hydroCharacteristics = state.problemeHebdo
                                             ->CaracteristiquesHydrauliques[state.area->index];

        const double waterValueCost = hydroCharacteristics.WeeklyWaterValueStateRegular
                                      * (hourlyResults.TurbinageHoraire[hourInWeek]
                                         - state.area->hydro.pumpingEfficiency
                                             * hourlyResults.PompageHoraire[hourInWeek]);

        const double storageReserveCost = state.reserveData
                                            ? state.reserveData.value()
                                                .at(state.area->index)
                                                .STStorageClusterReserveParticipationCostForYear
                                                  [state.hourInTheYear]
                                            : 0.0;

        const double hydroReserveCost = state.reserveData ? state.reserveData.value()
                                                              .at(state.area->index)
                                                              .HydroReserveParticipationCostForYear
                                                                [state.hourInTheYear]
                                                          : 0.0;

        return waterValueCost + storageReserveCost + hydroReserveCost;
    }

    static double calculateThermalCosts(const State& state)
    {
        double totalCost = 0.0;

        for (const auto& cluster: state.area->thermal.list.each_enabled())
        {
            totalCost += state.thermal[state.area->index]
                           .thermalClustersOperatingCost[cluster->enabledIndex];
        }

        return totalCost;
    }

    template<class Aux>
    static void setHourlyValue(IntermediateValues& iv, Aux&, const State& state, unsigned int)
    {
        iv[state.hourInTheYear] += calculateEnergyDeficitCosts(state) + calculateHydroCosts(state)
                                   + calculateThermalCosts(state);
    }

    static void yearEndBuildForEachThermalCluster(IntermediateValues& values,
                                                  State& state,
                                                  uint /*year*/,
                                                  unsigned int /*numSpace*/)
    {
        for (unsigned int i = 0; i < state.study.runtime.rangeLimits.hour[Data::rangeCount]; ++i)
        {
            values[i] += state.thermalClusterOperatingCostForYear[i];
        }
    }
};

/*!
** \brief Overall cost over all MC years (adequacy)
*/
using OverallCost = Economy::EconomyVariableBase<OverallCostTraits>;

} // namespace Adequacy
} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__
