// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "STStorageByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct STStorageInjectionByClusterTraits
{
    static std::string Caption()
    {
        return "STS inj by plant";
    }

    static std::string Unit()
    {
        return "P-injection - MW";
    }

    static std::string Description()
    {
        return "Energy injected by ST storage clusters";
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }

    static void setHourlyValue(const std::vector<IntermediateValues>& clusterValues,
                               const State& state)
    {
        for (uint clusterIndex = 0; clusterIndex != state.area->shortTermStorage.count();
             ++clusterIndex)
        {
            clusterValues[clusterIndex].hour[state.hourInTheYear]
              = state.hourlyResults->ShortTermStorage[clusterIndex].injection[state.hourInTheWeek];
        }
    }
};

using VCardSTstorageInjectionByCluster = VCardSTStorageByClusterBase<
  STStorageInjectionByClusterTraits>;

template<class NextT = Container::EndOfList>
using STstorageInjectionByCluster = STStorageByClusterBase<STStorageInjectionByClusterTraits,
                                                           NextT>;

} // End namespace Antares::Solver::Variable::Economy
