// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct PriceCSRTraits
{
    static std::string Caption()
    {
        return "MRG. PRICE CSR";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Marginal Price CSR, throughout all MC years";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateAverage;
    static constexpr uint8_t spatialAggregatePostProcessing = Category::
      spatialAggregatePostProcessingPrice;

    static double value(const State& state)
    {
        return -state.hourlyResults->CoutsMarginauxHorairesCSR[state.hourInTheWeek];
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        iv[state.hourInTheYear] = value(state);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
    }
};

using PriceCSR = EconomyVariableBase<PriceCSRTraits>;

} // namespace Antares::Solver::Variable::Economy
