// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "lold_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLD_CSRTraits: public LOLD_Base_Traits
{
    static std::string Caption()
    {
        return "LOLD CSR";
    }

    static std::string Description()
    {
        return "LOLD for CSR";
    }

    static bool checkCondition(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek]
               > 0.5;
    }
};

using VCardLOLD_CSR = VCard_Base<LOLD_CSRTraits>;

template<class NextT = Container::EndOfList>
using LOLD_CSR = Economy_Base<LOLD_CSRTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
