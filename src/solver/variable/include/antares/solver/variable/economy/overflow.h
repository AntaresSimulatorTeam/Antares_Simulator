// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct OverflowsTraits
{
    static std::string Caption()
    {
        return "H. OVFL";
    }

    static std::string Unit()
    {
        return "%";
    }

    static std::string Description()
    {
        return "Hydro overflow";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t isPossiblyNonApplicable = 1;

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               typename detail::AuxiliaryDataType<OverflowsTraits>::type&,
                               const State& state,
                               unsigned int)
    {
        intermediateValues.hour[state.hourInTheYear] = state.hourlyResults->debordementsHoraires
                                                         [state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using Overflows = EconomyVariableBase<OverflowsTraits>;

} // namespace Antares::Solver::Variable::Economy
