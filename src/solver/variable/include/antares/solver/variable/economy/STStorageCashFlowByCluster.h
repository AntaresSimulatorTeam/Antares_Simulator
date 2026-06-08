// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "STStorageByCluster_base.h"

namespace Antares::Solver::Variable::Economy
{
struct STstorageCashFlowByClusterTraits
{
    static std::string Caption()
    {
        return "STS Cashflow By Cluster";
    }

    static std::string Unit()
    {
        return "CashFlow - Euro";
    }

    static std::string Description()
    {
        return "Cash Flow by short term storage";
    }

    static constexpr uint8_t fileLevel = Category::FileLevel::de_sts;

    static size_t clusterCount(const Data::Area* area)
    {
        return area->shortTermStorage.count();
    }

    static void computeStats(IntermediateValues& iv)
    {
        iv.computeStatisticsForTheCurrentYear();
    }

    static void setHourlyValue(std::vector<IntermediateValues>& clusterValues,
                               State& state,
                               [[maybe_unused]] unsigned int numSpace)
    {
        const unsigned int hourInYear = state.hourInTheYear;
        for (uint clusterIndex = 0; clusterIndex != state.area->shortTermStorage.count();
             ++clusterIndex)
        {
            const auto& stsHourlyResults = state.hourlyResults->ShortTermStorage[clusterIndex];
            // CashFlow[h] = (withdrawal - injection) * MRG. PRICE
            // Note: The marginal price provided by the solver is negative (naming convention).
            clusterValues[clusterIndex].hour[hourInYear]
              = (stsHourlyResults.withdrawal[state.hourInTheWeek]
                 - stsHourlyResults.injection[state.hourInTheWeek])
                * (-state.hourlyResults->CoutsMarginauxHoraires[state.hourInTheWeek]);
        }
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
                VCardSTStorageByClusterBase<STstorageCashFlowByClusterTraits>>(results,
                                                                               fileLevel,
                                                                               precision);
            clusterIndex++;
        }
    }
};

/*!
** \brief Cash flow by short term storage clusters
*/
using STstorageCashFlowByCluster = STStorageByClusterBase<STstorageCashFlowByClusterTraits>;

} // End namespace Antares::Solver::Variable::Economy
