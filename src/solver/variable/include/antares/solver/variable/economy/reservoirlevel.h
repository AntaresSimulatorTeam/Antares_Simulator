// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct ReservoirLevelTraits
{
    static std::string Caption()
    {
        return "H. LEV";
    }

    static std::string Unit()
    {
        return "%";
    }

    static std::string Description()
    {
        return "Hydro Level";
    }

    using ResultsProfile = StandardResults<>;

    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;
    static constexpr uint8_t isPossiblyNonApplicable = 1;

    using AuxiliaryDataType = double;

    static void initializeFromArea(AuxiliaryDataType& capacity, Data::Study*, Data::Area* area)
    {
        capacity = area->hydro.reservoirCapacity;
    }

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               AuxiliaryDataType& capacity,
                               const State& state,
                               unsigned int)
    {
        intermediateValues[state.hourInTheYear] = state.hourlyResults
                                                    ->niveauxHoraires[state.hourInTheWeek]
                                                  / capacity * 100.0;
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
    }
};

using ReservoirLevel = EconomyVariableBase<ReservoirLevelTraits>;

} // namespace Antares::Solver::Variable::Economy
