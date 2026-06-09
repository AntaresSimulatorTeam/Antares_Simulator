// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_HURDLE_COSTS_H__
#define __SOLVER_VARIABLE_ECONOMY_HURDLE_COSTS_H__

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct HurdleCostsTraits
{
    static std::string Caption()
    {
        return "HURDLE COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Hurdle costs, over all MC years";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static void hourForEachLink(IntermediateValues& iv, State& state)
    {
        if (state.link->useHurdlesCost)
        {
            const double flowLinear = state.ntc.ValeurDuFlux[state.link->index];

            if (state.link->useLoopFlow)
            {
                const double loopFlow = state.problemeHebdo->ValeursDeNTC[state.hourInTheWeek]
                                          .ValeurDeLoopFlowOrigineVersExtremite[state.link->index];
                if (flowLinear - loopFlow > 0.)
                {
                    const double hurdleCostDirect = (flowLinear - loopFlow)
                                                    * state.link->parameters
                                                        .entry[Data::fhlHurdlesCostDirect]
                                                              [state.hourInTheYear];
                    iv.hour[state.hourInTheYear] += hurdleCostDirect;
                    state.annualSystemCost += hurdleCostDirect;
                }
                else
                {
                    const double hurdleCostIndirect = -(flowLinear - loopFlow)
                                                      * state.link->parameters
                                                          .entry[Data::fhlHurdlesCostIndirect]
                                                                [state.hourInTheYear];
                    iv.hour[state.hourInTheYear] += hurdleCostIndirect;
                    state.annualSystemCost += hurdleCostIndirect;
                }
            }
            else
            {
                if (flowLinear > 0.)
                {
                    const double hurdleCostDirect = flowLinear
                                                    * state.link->parameters
                                                        .entry[Data::fhlHurdlesCostDirect]
                                                              [state.hourInTheYear];
                    iv.hour[state.hourInTheYear] += hurdleCostDirect;
                    state.annualSystemCost += hurdleCostDirect;
                }
                else
                {
                    const double hurdleCostIndirect = -flowLinear
                                                      * state.link->parameters
                                                          .entry[Data::fhlHurdlesCostIndirect]
                                                                [state.hourInTheYear];
                    iv.hour[state.hourInTheYear] += hurdleCostIndirect;
                    state.annualSystemCost += hurdleCostIndirect;
                }
            }
        }
    }
};

/*!
** \brief Marginal HurdleCosts
*/
using HurdleCosts = EconomyLink_Base<HurdleCostsTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_HURDLE_COSTS_H__
