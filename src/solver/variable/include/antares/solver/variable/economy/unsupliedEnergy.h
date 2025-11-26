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
#include "antares/solver/variable/categories.h"
#include "antares/solver/variable/state.h"
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/storage/results.h"

namespace Antares::Solver::Variable::Economy
{

struct UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Unsuplied Energy (demand that cannot be satisfied)";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;
    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void yearBeginLogic(unsigned, IntermediateValues& iv, Data::Area*, unsigned)
    {
        iv.reset();
    }

    static void setValue(const State& state, IntermediateValues& iv, unsigned)
    {
        iv[state.hourInTheYear] = state.hourlyResults
                                    ->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static bool checkCondition(const State&)
    {
        return true;
    }

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek];
    }
};

struct UnsuppliedEnergyCSRTrait: UnsuppliedEnergyTrait
{
    static std::string Caption()
    {
        return "UNSP. ENRG CSR";
    }

    static std::string Description()
    {
        return "Unsuplied Energy after CSR (demand that cannot be satisfied)";
    }

    static void setValue(const State& state, IntermediateValues& iv, unsigned)
    {
        iv[state.hourInTheYear] = state.hourlyResults
                                    ->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek];
    }

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek];
    }
};

template<class NextT = Container::EndOfList>
using UnsupliedEnergy = Economy_Base<UnsuppliedEnergyTrait, NextT>;

template<class NextT = Container::EndOfList>
using UnsupliedEnergyCSR = Economy_Base<UnsuppliedEnergyCSRTrait, NextT>;

} // namespace Antares::Solver::Variable::Economy
