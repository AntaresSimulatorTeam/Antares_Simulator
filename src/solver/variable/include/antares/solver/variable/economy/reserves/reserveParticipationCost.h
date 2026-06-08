// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "antares/solver/variable/economy/economy_base.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

struct ReserveParticipationCostTraits
{
    static std::string Caption()
    {
        return "RESERVE PARTICIPATION COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Reserve Participation Cost throughout all MC years of all clusters : Thermal, "
               "hydro and short term";
    }

    using ResultsProfile = StandardResults<R::AllYears::Average>;

    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    template<class AuxiliaryData>
    static void setHourlyValue(IntermediateValues& values,
                               AuxiliaryData& /*auxiliaryData*/,
                               const State& state,
                               unsigned int /*numSpace*/)
    {
        if (state.reserveData)
        {
            values[state.hourInTheYear] = state.reserveData.value()
                                            .at(state.area->index)
                                            .reserveParticipationCostForYear[state.hourInTheYear];
        }
        else
        {
            values[state.hourInTheYear] = 0;
        }
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }
};

using ReserveParticipationCost = EconomyVariableBase<ReserveParticipationCostTraits>;

} // namespace Antares::Solver::Variable::Economy::Reserves
