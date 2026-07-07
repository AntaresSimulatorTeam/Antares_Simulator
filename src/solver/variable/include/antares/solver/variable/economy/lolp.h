// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef SOLVER_VARIABLE_ECONOMY_LOLP_H__
#define SOLVER_VARIABLE_ECONOMY_LOLP_H__

#include "economy_base.h"
#include "lolp_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLPTraits: public LOLP_Base_Traits
{
    static std::string Caption()
    {
        return "LOLP";
    }

    static std::string Description()
    {
        return "LOLP";
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        if (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek] > 0.)
        {
            iv[state.hourInTheYear] = value(state);
        }
    }
};

using LOLP = EconomyVariableBase<LOLPTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // SOLVER_VARIABLE_ECONOMY_LOLP_H__
