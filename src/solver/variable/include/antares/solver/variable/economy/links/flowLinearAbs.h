// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_FlowLinearAbs_H__
#define __SOLVER_VARIABLE_ECONOMY_FlowLinearAbs_H__

#include <cmath>

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct FlowLinearAbsTraits
{
    static std::string Caption()
    {
        return "UCAP LIN.";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Used capacity assessed, over all MC years, through linear optimization";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static double hourValue(State& state)
    {
        return std::abs(state.ntc.ValeurDuFlux[state.link->index]);
    }
};

/*!
** \brief Marginal FlowLinearAbs
*/
using FlowLinearAbs = EconomyLink_Base<FlowLinearAbsTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_FlowLinearAbs_H__
