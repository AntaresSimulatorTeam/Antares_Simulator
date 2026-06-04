// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "economy_base.h"
#include "lolp_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLP_CSRTraits: public LOLP_Base_Traits
{
    static std::string Caption()
    {
        return "LOLP CSR";
    }

    static std::string Description()
    {
        return "LOLP for CSR";
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        if (state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek] > 0.0)
        {
            iv[state.hourInTheYear] = value(state);
        }
    }
};

using LOLP_CSR = EconomyVariableBase<LOLP_CSRTraits>;

} // namespace Antares::Solver::Variable::Economy
