// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

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

    static bool checkCondition(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek]
               > 0.0;
    }
};

using VCardLOLP_CSR = VCard_Base<LOLP_CSRTraits>;

template<class NextT = Container::EndOfList>
using LOLP_CSR = Economy_Base<LOLP_CSRTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
