// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct SpilledEnergyTraits
{
    static std::string Caption()
    {
        return "SPIL. ENRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Spilled Energy (generation that cannot be satisfied)";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value(const State& state)
    {
        assert(state.hourlyResults && "Invalid pointer to simplex results");
        return state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek];
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

using VCardSpilledEnergy = EconomyVariableCard<SpilledEnergyTraits>;

/*!
** \brief C02 Average value of the overrall SpilledEnergy emissions expected from all
**   the thermal dispatchable clusters
*/
using SpilledEnergy = EconomyVariableBase<SpilledEnergyTraits>;

} // namespace Antares::Solver::Variable::Economy
