// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "DispatchablePlantByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct NonProportionalCostByDispatchablePlantTraits
{
    static std::string Caption()
    {
        return "NP Cost by plant";
    }

    static std::string Unit()
    {
        return "NP Cost - Euro";
    }

    static std::string Description()
    {
        return "Non proportional costs by all the clusters";
    }

    static void yearEndBuildForEachThermalCluster(
      std::vector<std::vector<IntermediateValues>>& yearlyValues,
      State& state,
      uint,
      unsigned int numSpace)
    {
        for (unsigned int i = state.study.runtime.rangeLimits.hour[Data::rangeBegin];
             i <= state.study.runtime.rangeLimits.hour[Data::rangeEnd];
             ++i)
        {
            yearlyValues[numSpace][state.thermalCluster->enabledIndex].hour[i]
              = state.thermalClusterNonProportionalCostForYear[i];
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

/*!
** \brief NP cost expected from all the thermal dispatchable clusters
*/
using NonProportionalCostByDispatchablePlant = DispatchablePlantByClusterBase<
  NonProportionalCostByDispatchablePlantTraits>;

} // namespace Antares::Solver::Variable::Economy
