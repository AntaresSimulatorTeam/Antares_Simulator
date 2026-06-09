// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct HydroStorageTraits
{
    static std::string Caption()
    {
        return "H. STOR";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Hydro Storage Generation";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               typename detail::AuxiliaryDataType<HydroStorageTraits>::type&,
                               const State& state,
                               unsigned int)
    {
        intermediateValues.hour[state.hourInTheYear] = state.hourlyResults
                                                         ->TurbinageHoraire[state.hourInTheWeek];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardHydroStorage = EconomyVariableCard<HydroStorageTraits>;

using HydroStorage = EconomyVariableBase<HydroStorageTraits>;

} // namespace Antares::Solver::Variable::Economy
