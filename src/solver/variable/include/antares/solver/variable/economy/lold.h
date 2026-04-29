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

    static bool checkCondition(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek] > 0.5;
    }
};

using VCardLOLD = VCard_Base<LOLDTraits>;

template<class NextT = Container::EndOfList>
using LOLD = Economy_Base<LOLDTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
