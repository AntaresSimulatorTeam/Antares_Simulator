/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLP_CSRTraits
{
    static std::string Caption()
    {
        return "LOLP CSR";
    }

    static std::string Unit()
    {
        return "%";
    }

    static std::string Description()
    {
        return "LOLP for CSR";
    }

    typedef Results<R::AllYears::Average<>> ResultsType;

    static constexpr uint8_t decimal = 2;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value()
    {
        return 100.;
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsOrForTheCurrentYear();
    }

    static bool checkCondition(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek]
               > 0.5;
    }
};

using VCardLOLP_CSR = VCard_Base<LOLP_CSRTraits>;

template<class NextT = Container::EndOfList>
using LOLP_CSR = Economy_Base<LOLP_CSRTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
