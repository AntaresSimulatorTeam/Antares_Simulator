// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "STStorageByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct STStorageLevelsByClusterTraits
{
    static std::string Caption()
    {
        return "STS lvl by plant";
    }

    static std::string Unit()
    {
        return "Levels - MWh";
    }

    static std::string Description()
    {
        return "Levels by ST storage clusters";
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeAveragesForCurrentYearFromHourlyResults();
    }

    static void setHourlyValue(const std::vector<IntermediateValues>& clusterValues,
                               const State& state)
    {
        for (uint clusterIndex = 0; clusterIndex != state.area->shortTermStorage.count();
             ++clusterIndex)
        {
            clusterValues[clusterIndex].hour[state.hourInTheYear] = state.hourlyResults
                                                                      ->ShortTermStorage
                                                                        [clusterIndex]
                                                                      .level[state.hourInTheWeek];
        }
    }
};

using VCardSTstorageLevelsByCluster = VCardSTStorageByClusterBase<STStorageLevelsByClusterTraits>;

template<class NextT = Container::EndOfList>
using STstorageLevelsByCluster = STStorageByClusterBase<STStorageLevelsByClusterTraits, NextT>;

} // End namespace Antares::Solver::Variable::Economy
