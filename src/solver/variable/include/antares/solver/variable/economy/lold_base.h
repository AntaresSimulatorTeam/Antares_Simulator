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
#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

template<bool isCSR>
struct LOLD_BaseTraits
{
    static std::string Caption()
    {
        return isCSR ? "LOLD CSR" : "LOLD";
    }

    static std::string Unit()
    {
        return "Hours";
    }

    static std::string Description()
    {
        return isCSR ? "LOLD for CSR" : "LOLD";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    static constexpr uint8_t decimal = 4;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSumThen1IfPositive;

    static double value(const State&)
    {
        return 1.;
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static bool checkCondition(const State& state)
    {
        if constexpr (isCSR)
        {
            return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek]
                   > 0.5;
        }
        else
        {
            return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek]
                   > 0.5;
        }
    }
};

using LOLDTraits = LOLD_BaseTraits<false>;
using LOLD_CSRTraits = LOLD_BaseTraits<true>;

} // namespace Antares::Solver::Variable::Economy
