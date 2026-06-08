// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/area/scratchpad.h>

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct DispatchableGenMarginTraits
{
    static std::string Caption()
    {
        return "DTG MRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Dispatchable Generation Margin";
    }

    using ResultsProfile = StandardResults<>;
    using ResultsType = ResultsProfile;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void setHourlyValue(IntermediateValues& yearlyValues, const State& state, unsigned int)
    {
        yearlyValues[state.hourInTheYear] += state.scratchpad
                                               ->dispatchableGenerationMargin[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardDispatchableGenMargin = EconomyVariableCard<DispatchableGenMarginTraits>;

/*!
** \brief Marginal DispatchableGenMargin
*/
using DispatchableGenMargin = EconomyVariableBase<DispatchableGenMarginTraits>;

} // namespace Antares::Solver::Variable::Economy
