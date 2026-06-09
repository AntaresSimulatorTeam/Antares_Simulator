// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct ResidualLoadTraits
{
    static std::string Caption()
    {
        return "RES LOAD";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Residual load";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Antares::Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static double value(AuxiliaryDataType area, const State& state)
    {
        return state.problemeHebdo->ConsommationsAbattues[state.hourInTheWeek]
          .ConsommationAbattueDuPays[area->index];
    }

    static void setHourlyValue(IntermediateValues& iv,
                               AuxiliaryDataType area,
                               const State& state,
                               unsigned int)
    {
        if (area != nullptr)
        {
            iv[state.hourInTheYear] = value(area, state);
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using ResidualLoad = EconomyVariableBase<ResidualLoadTraits>;

} // namespace Antares::Solver::Variable::Economy
