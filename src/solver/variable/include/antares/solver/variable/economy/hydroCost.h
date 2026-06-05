// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct HydroCostTraits
{
    static std::string Caption()
    {
        return "H. COST";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Hydro Cost throughout all MC years, of all the thermal dispatchable clusters";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = double;

    static void initializeFromArea(AuxiliaryDataType& pumpRatio, Data::Study*, Data::Area* area)
    {
        pumpRatio = area->hydro.pumpingEfficiency;
    }

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               AuxiliaryDataType& pumpRatio,
                               const State& state,
                               unsigned int)
    {
        intermediateValues.hour[state.hourInTheYear] += state.hourlyResults
                                                          ->valeurH2oHoraire[state.hourInTheWeek]
                                                        * (state.hourlyResults
                                                             ->TurbinageHoraire[state.hourInTheWeek]
                                                           - pumpRatio
                                                               * state.hourlyResults->PompageHoraire
                                                                   [state.hourInTheWeek]);
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using HydroCost = EconomyVariableBase<HydroCostTraits>;

} // namespace Antares::Solver::Variable::Economy
