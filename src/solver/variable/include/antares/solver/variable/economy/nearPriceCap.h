// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

constexpr double eps = 1e-2;
constexpr double margin = 5.;

struct NearPriceCapTraits
{
    static std::string Caption()
    {
        return "NPCAP HOURS";
    }

    static std::string Unit()
    {
        return "Hours";
    }

    static std::string Description()
    {
        return "Near Price Cap Hours";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 4;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSumThen1IfPositive;

    using AuxiliaryDataType = double;

    static void initializeFromArea(AuxiliaryDataType& unsuppliedEnergyCost,
                                   Data::Study*,
                                   Data::Area* area)
    {
        unsuppliedEnergyCost = area->thermal.unsuppliedEnergyCost;
    }

    static void setHourlyValue(IntermediateValues& iv,
                               AuxiliaryDataType unsuppliedEnergyCost,
                               const State& state,
                               unsigned int)
    {
        double mrgPrice = -state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek];
        iv[state.hourInTheYear] = (mrgPrice > unsuppliedEnergyCost - margin + eps) ? 1. : 0.;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using NearPriceCap = EconomyVariableBase<NearPriceCapTraits>;

} // namespace Antares::Solver::Variable::Economy
