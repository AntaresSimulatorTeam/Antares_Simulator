// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{
struct AvailableDispatchGenTraits
{
    static std::string Caption()
    {
        return "AVL DTG";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Available dispatchable generation";
    }

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static void yearBegin(IntermediateValues& yearlyValues,
                          AuxiliaryDataType area,
                          unsigned int year,
                          unsigned int)
    {
        if (!area)
        {
            return;
        }

        // Somme de toutes les productions disponibles pour l'ensemble des paliers thermiques.
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            for (unsigned int hour = 0; hour != cluster->series.timeSeries.height; ++hour)
            {
                yearlyValues.hour[hour] += availableProduction[hour];
            }
        }
    }

    static bool checkCondition(const State&)
    {
        return false;
    }

    static double value(const State&)
    {
        return 0.;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using VCardAvailableDispatchGen = VCard_Base<AvailableDispatchGenTraits>;

/*!
** \brief Marginal AvailableDispatchGen
*/
template<class NextT = Container::EndOfList>
using AvailableDispatchGen = Economy_Base<AvailableDispatchGenTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
