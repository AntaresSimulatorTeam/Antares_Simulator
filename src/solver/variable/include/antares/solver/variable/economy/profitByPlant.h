// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct ProfitByPlantTraits
{
    static std::string Caption()
    {
        return "Profit by plant";
    }

    static std::string Unit()
    {
        return "Profit - Euro";
    }

    static std::string Description()
    {
        return "Profit for thermal units";
    }

    static void setHourlyValuesForCurrentYear(std::vector<IntermediateValues>& clusterValues,
                                              State& state)
    {
        auto area = state.area;
        auto& thermal = state.thermal;
        const std::vector<double>& areaMarginalCosts = state.hourlyResults->CoutsMarginauxHoraires;
        uint hourInTheWeek = state.hourInTheWeek;
        uint hourInTheYear = state.hourInTheYear;

        for (auto& cluster: area->thermal.list.each_enabled())
        {
            double hourlyClusterProduction = thermal[area->index]
                                               .thermalClustersProductions[cluster->enabledIndex];
            uint tsIndex = cluster->series.timeseriesNumbers[state.year];

            clusterValues[cluster->enabledIndex].hour[hourInTheYear]
              = std::max((hourlyClusterProduction - cluster->PthetaInf[hourInTheYear]), 0.)
                * (-areaMarginalCosts[hourInTheWeek]
                   - cluster->getCostProvider().getMarginalCost(tsIndex, hourInTheYear));
        }
    }

    static Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      const std::vector<std::vector<IntermediateValues>>& yearlyValues,
      unsigned int,
      unsigned int numSpace)
    {
        return yearlyValues[numSpace][0].hour;
    }
};

using VCardProfitByPlant = VCardDispatchablePlantByClusterBase<ProfitByPlantTraits>;

/*!
** \brief Profit by plant for thermal dispatchable clusters
*/
template<class NextT = Container::EndOfList>
using ProfitByPlant = DispatchablePlantByClusterBase<ProfitByPlantTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
