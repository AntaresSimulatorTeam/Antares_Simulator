// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"
#include "lold_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLDTraits: public LOLD_Base_Traits
{
    static std::string Caption()
    {
        return "LOLD";
    }

    static std::string Description()
    {
        return "LOLD";
    }

    static void setHourlyValue(IntermediateValues& iv, const State& state, unsigned int)
    {
        if (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek] > 0.5)
        {
            iv[state.hourInTheYear] = value(state);
        }
    }
};

using VCardLOLD = EconomyVariableCard<LOLDTraits>;

using LOLD = EconomyVariableBase<LOLDTraits>;

} // namespace Antares::Solver::Variable::Economy
