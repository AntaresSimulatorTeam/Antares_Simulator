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

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct SpilledEnergyTraits: public UnitMWH
{
    static std::string Caption()
    {
        return "SPIL. ENRG";
    }

    static std::string Description()
    {
        return "Spilled Energy (generation that cannot be satisfied)";
    }

    typedef Results<R::AllYears::Average< // The average values throughout all years
      R::AllYears::StdDeviation<          // The standard deviation values throughout all years
        R::AllYears::Min<                 // The minimum values throughout all years
          R::AllYears::Max<               // The maximum values throughout all years
            >>>>>
      ResultsType;

    static constexpr uint8_t decimal = 0;

    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static double value(const State& state)
    {
        return state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static bool checkCondition(const State&)
    {
        return true;
    }
};

typedef VCard_Base<SpilledEnergyTraits> VCardSpilledEnergy;

template<class NextT = Container::EndOfList>
using SpilledEnergy = Economy_Base<SpilledEnergyTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
