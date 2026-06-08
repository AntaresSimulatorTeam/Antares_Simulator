// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_CongestionFee_H__
#define __SOLVER_VARIABLE_ECONOMY_CongestionFee_H__

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct CongestionFeeTraits
{
    static std::string Caption()
    {
        return "CONG. FEE (ALG.)";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Congestion fee collected throughout all MC years (Alg.)";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static double hourValue(State& state, double upstreamPrice, double downstreamPrice)
    {
        return state.ntc.ValeurDuFlux[state.link->index] * (upstreamPrice - downstreamPrice);
    }
};

/*!
** \brief Marginal CongestionFee
*/
using CongestionFee = EconomyLink_Base<CongestionFeeTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_CongestionFee_H__
