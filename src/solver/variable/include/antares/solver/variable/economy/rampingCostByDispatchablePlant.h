// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
class RampingCostByDispatchablePlantTraits
{
public:
    static std::string Caption()
    {
        return "RAMP COST BY PLANT";
    }

    static std::string Unit()
    {
        return "Ramping Cost - Euro";
    }

    static std::string Description()
    {
        return "Ramping costs by all the clusters";
    }

    static void setHourlyValue(std::vector<IntermediateValues>& clusterValues,
                               State& state,
                               [[maybe_unused]] unsigned int numSpace)
    {
        auto area = state.area;
        auto& thermal = state.thermal;
        for (auto& cluster: area->thermal.list.each_enabled())
        {
            clusterValues[cluster->enabledIndex].hour[state.hourInTheYear]
              = thermal[area->index].thermalClustersRampingCost[cluster->enabledIndex];
        }
    }

    static void yearEndBuildPrepareDataForEachThermalCluster(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint,
      unsigned int numSpace)
    {
        for (unsigned int i = 0; i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd]; ++i)
        {
            state.thermalClusterRampingCostForYear[i] = yearlyValues[numSpace][state.thermalCluster
                                                                                 ->enabledIndex]
                                                          .hour[i];
        }
    }

    static void yearEndBuildForEachThermalCluster(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint,
      unsigned int numSpace)
    {
        // Get end year calculations
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            yearlyValues[numSpace][state.thermalCluster->enabledIndex].hour[i]
              = state.thermalClusterRampingCostForYear[i];
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

using RampingCostByDispatchablePlant = DispatchablePlantByClusterBase<
  RampingCostByDispatchablePlantTraits>;

} // namespace Antares::Solver::Variable::Economy
