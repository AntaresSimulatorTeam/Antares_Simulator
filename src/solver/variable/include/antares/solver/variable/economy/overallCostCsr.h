// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct OverallCostCsrTraits
{
    static std::string Caption()
    {
        return "OV. COST CSR";
    }

    static std::string Unit()
    {
        return "Euro";
    }

    static std::string Description()
    {
        return "Overall Cost throughout all MC years";
    }

    using ResultsProfile = Results<std::tuple<R::AllYears::Average>, R::AllYears::Average>;

    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

    using AuxiliaryDataType = Antares::Data::Area*;

    static void initializeFromArea(AuxiliaryDataType& area, Data::Study*, Data::Area* inputArea)
    {
        area = inputArea;
    }

    static void setHourlyValue(IntermediateValues& intermediateValues,
                               AuxiliaryDataType& area,
                               const State& state,
                               unsigned int)
    {
        const double costForSpilledOrUnsuppliedEnergyCSR
          = (state.hourlyResults->ValeursHorairesDeDefaillancePositiveCSR[state.hourInTheWeek]
             * area->thermal.unsuppliedEnergyCost)
            + (state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
               * area->thermal.spilledEnergyCost);

        intermediateValues[state.hourInTheYear] += costForSpilledOrUnsuppliedEnergyCSR;
    }

    static void yearEndBuildForEachThermalCluster(IntermediateValues& values,
                                                  State& state,
                                                  uint,
                                                  unsigned int)
    {
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            values[i] += state.thermalClusterOperatingCostForYear[i];
        }
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }
};

using OverallCostCsr = EconomyVariableBase<OverallCostCsrTraits>;

} // namespace Antares::Solver::Variable::Economy
