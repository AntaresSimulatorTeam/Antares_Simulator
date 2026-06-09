// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__
#define __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct FlowLinearTraits
{
    static std::string Caption()
    {
        return "FLOW LIN.";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Flow assessed, over all MC years, through linear optimization";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static double hourValue(State& state)
    {
        return state.ntc.ValeurDuFlux[state.link->index];
    }

    template<class R>
    static void buildDigest(SurveyResults& results,
                            int digestLevel,
                            int dataLevel,
                            const R& pResults)
    {
        if (dataLevel & Category::DataLevel::link)
        {
            if (digestLevel & Category::digestFlowLinear)
            {
                results.data.matrix
                  .entry[results.data.link->from->index][results.data.link->with->index]
                  = pResults.avgdata().year;
                results.data.matrix
                  .entry[results.data.link->with->index][results.data.link->from->index]
                  = -pResults.avgdata().year;
            }
        }
    }
};

/*!
** \brief Marginal FlowLinear
*/
using FlowLinear = EconomyLink_Base<FlowLinearTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_FlowLinear_H__
