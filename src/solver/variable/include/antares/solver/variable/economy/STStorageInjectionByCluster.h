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

    static constexpr uint8_t fileLevel = Category::FileLevel::de_sts;

    static size_t clusterCount(const Data::Area* area)
    {
        return area->shortTermStorage.count();
    }

    static void computeStats(IntermediateValues& intermediateValues)
    {
        intermediateValues.computeStatisticsForTheCurrentYear();
    }

    static void buildSurveyReport(const std::vector<IntermediateValues>& clusterValues,
                                  SurveyResults& results,
                                  int fileLevel,
                                  int precision)
    {
        const auto& shortTermStorage = results.data.area->shortTermStorage;
        uint clusterIndex = 0;
        for (const auto& sts: shortTermStorage.storagesByIndex)
        {
            results.variableCaption = sts.properties.name;
            results.variableUnit = Unit();
            clusterValues[clusterIndex]
              .template buildAnnualSurveyReport<
                VCardSTStorageByClusterBase<STStorageInjectionByClusterTraits>>(results,
                                                                                fileLevel,
                                                                                precision);
            clusterIndex++;
        }
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

using STstorageInjectionByCluster = STStorageByClusterBase<STStorageInjectionByClusterTraits>;

} // End namespace Antares::Solver::Variable::Economy
