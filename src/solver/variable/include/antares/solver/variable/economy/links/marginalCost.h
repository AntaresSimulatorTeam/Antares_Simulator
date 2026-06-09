// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_MarginalCost_H__
#define __SOLVER_VARIABLE_ECONOMY_MarginalCost_H__

#include <cmath>

#include "links_base.h"

namespace Antares::Solver::Variable::Economy
{
struct MarginalCostTraits
{
    static std::string Caption()
    {
        return "MARG. COST";
    }

    static std::string Unit()
    {
        return "Euro/MW";
    }

    static std::string Description()
    {
        return "Decrease of the overall operating cost expected by a 1MW capacity reinforcement";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 2;

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeAveragesForCurrentYearFromHourlyResults();
    }

    static void hourForEachLink(IntermediateValues& iv, State& state)
    {
        iv[state.hourInTheYear] = std::abs(
          state.problemeHebdo->VariablesDualesDesContraintesDeNTC[state.hourInTheWeek]
            .VariableDualeParInterconnexion[state.link->index]);

        const double flow = state.problemeHebdo->ValeursDeNTC[state.hourInTheWeek]
                              .ValeurDuFlux[state.link->index];
        const double flow_lowerbound = -state.problemeHebdo->ValeursDeNTC[state.hourInTheWeek]
                                          .ValeurDeNTCExtremiteVersOrigine[state.link->index];
        const double flow_upperbound = state.problemeHebdo->ValeursDeNTC[state.hourInTheWeek]
                                         .ValeurDeNTCOrigineVersExtremite[state.link->index];

        if (flow - 0.001 > flow_lowerbound && flow + 0.001 < flow_upperbound)
        {
            iv[state.hourInTheYear] = 0.;
        }
    }
};

/*!
** \brief Marginal MarginalCost
*/
using MarginalCost = EconomyLink_Base<MarginalCostTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_MarginalCost_H__
