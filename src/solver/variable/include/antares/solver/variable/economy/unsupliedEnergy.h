// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Unsuplied Energy (demand that cannot be satisfied)";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        iv[state.hourInTheYear] = value(state);
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }
};

struct UnsuppliedEnergyCSRTrait: UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG CSR";
    }

    static std::string Description()
    {
        return "Unsuplied Energy after CSR (demand that cannot be satisfied)";
    }

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        iv[state.hourInTheYear] = value(state);
    }
};

using UnsupliedEnergy = EconomyVariableBase<UnsuppliedEnergyTrait>;

using UnsupliedEnergyCSR = EconomyVariableBase<UnsuppliedEnergyCSRTrait>;

} // namespace Antares::Solver::Variable::Economy
