/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef SOLVER_VARIABLE_ECONOMY_LOLP_H__
#define SOLVER_VARIABLE_ECONOMY_LOLP_H__

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLPTraits
{
    static std::string Caption()
    {
        return "LOLP";
    }

    static std::string Unit()
    {
        return "%";
    }

    static std::string Description()
    {
        return "LOLP";
    }

    typedef Results<R::AllYears::Average<>> ResultsType;

    static constexpr uint8_t decimal = 2;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateOr;

    static double value(const State&)
    {
        return 100.;
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsOrForTheCurrentYear();
    }

    static bool checkCondition(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek] > 0.;
    }
};

using VCardLOLP = VCard_Base<LOLPTraits>;

template<class NextT = Container::EndOfList>
using LOLP = Economy_Base<LOLPTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy

#endif // SOLVER_VARIABLE_ECONOMY_LOLP_H__
