// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct RampingCostByDispatchablePlantTraits
{
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
