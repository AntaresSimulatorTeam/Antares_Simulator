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

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Antares::Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static bool checkCondition(AuxiliaryDataType area, const State&)
    {
        return area != nullptr;
    }

    static double value(AuxiliaryDataType area, const State& state)
    {
        return state.problemeHebdo->ConsommationsAbattues[state.hourInTheWeek]
          .ConsommationAbattueDuPays[area->index];
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardResidualLoad = VCard_Base<ResidualLoadTraits>;

/*!
** \brief ResidualLoad
*/
template<class NextT = Container::EndOfList>
using ResidualLoad = Economy_Base<ResidualLoadTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
