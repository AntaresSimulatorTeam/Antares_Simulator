// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/dynamicAggregation/setData.h"

namespace Antares::Solver::Variable
{

SetDataAllYears::SetDataAllYears(const std::set<Data::Area*, Data::CompareAreaName>& set,
                                 Data::Study& study):
    SetDataBase(set)
{
    auto initAndReset =
      [&study](auto& avgVec, auto& stdDevVec, auto& minVec, auto& maxVec, size_t size)
    {
        avgVec.resize(size);
        stdDevVec.resize(size);
        minVec.resize(size);
        maxVec.resize(size);
        for (size_t i = 0; i < size; ++i)
        {
            minVec[i].reset();
            maxVec[i].reset();
            avgVec[i].initializeFromStudy(study);
            avgVec[i].reset();
            stdDevVec[i].initializeFromStudy(study);
            stdDevVec[i].reset();
        }
    };

    // Thermal
    initAndReset(averageThermal_,
                 stdDevThermal_,
                 minThermal_,
                 maxThermal_,
                 thermalGroupNames_.size());

    // Renewable
    initAndReset(averageRenewable_,
                 stdDevRenewable_,
                 minRenewable_,
                 maxRenewable_,
                 renewableGroupNames_.size());

    // STS
    initAndReset(averageStsInjection_,
                 stdDevStsInjection_,
                 minStsInjection_,
                 maxStsInjection_,
                 stsGroupNames_.size());
    initAndReset(averageStsWithdrawal_,
                 stdDevStsWithdrawal_,
                 minStsWithdrawal_,
                 maxStsWithdrawal_,
                 stsGroupNames_.size());
    initAndReset(averageStsLevel_,
                 stdDevStsLevel_,
                 minStsLevel_,
                 maxStsLevel_,
                 stsGroupNames_.size());
}

void SetDataAllYears::merge(const SetDataSingleYear& toMerge, Data::Study& study, unsigned year)
{
    IntermediateValues values;
    values.initializeFromStudy(study);

    // Helper function to process results
    auto processResults = [&values, &year](const auto& sourceResults,
                                           auto& minResults,
                                           auto& maxResults,
                                           auto& avgResults,
                                           auto& stdDevResults,
                                           auto computeFunc)
    {
        for (size_t i = 0; i < sourceResults.size(); ++i)
        {
            std::ranges::copy(sourceResults[i], values.hour);
            computeFunc(values);
            minResults[i].merge(year, values);
            maxResults[i].merge(year, values);
            avgResults[i].merge(year, values);
            stdDevResults[i].merge(year, values);
        }
    };

    // Thermal_
    processResults(toMerge.thermalResults_,
                   minThermal_,
                   maxThermal_,
                   averageThermal_,
                   stdDevThermal_,
                   [](IntermediateValues& v) { v.computeStatisticsForTheCurrentYear(); });

    // Renewable_
    processResults(toMerge.renewableResults_,
                   minRenewable_,
                   maxRenewable_,
                   averageRenewable_,
                   stdDevRenewable_,
                   [](IntermediateValues& v) { v.computeStatisticsForTheCurrentYear(); });

    // STS Injection_
    processResults(toMerge.stsInjectionResults_,
                   minStsInjection_,
                   maxStsInjection_,
                   averageStsInjection_,
                   stdDevStsInjection_,
                   [](IntermediateValues& v) { v.computeStatisticsForTheCurrentYear(); });

    // STS Withdrawal_
    processResults(toMerge.stsWithdrawalResults_,
                   minStsWithdrawal_,
                   maxStsWithdrawal_,
                   averageStsWithdrawal_,
                   stdDevStsWithdrawal_,
                   [](IntermediateValues& v) { v.computeStatisticsForTheCurrentYear(); });

    // STS Level_: we compute averages instead of sum
    processResults(toMerge.stsLevelResults_,
                   minStsLevel_,
                   maxStsLevel_,
                   averageStsLevel_,
                   stdDevStsLevel_,
                   [](IntermediateValues& v)
                   { v.computeAveragesForCurrentYearFromHourlyResults(); });
}

void SetDataAllYears::processGroups(const std::vector<R::AllYears::Average>& average,
                                    const std::vector<R::AllYears::StdDeviation>& stdDev,
                                    const std::vector<R::AllYears::Min>& min,
                                    const std::vector<R::AllYears::Max>& max,
                                    const std::set<std::string>& groupNames,
                                    const Category::Precision& precision,
                                    const std::string& suffix,
                                    SurveyResults& survey) const
{
    size_t index = 0;

    for (const auto& group: groupNames)
    {
        survey.variableCaption = group + suffix;
        survey.variableUnit = "MWh";

        IntermediateValues values;

        average[index].buildSurveyReport<IntermediateValues, VCardDynamic>(
          survey,
          values, // not used, placeholder for templates
          Category::DataLevel::setOfAreas,
          Category::FileLevel::va,
          precision);

        stdDev[index].buildSurveyReport<R::AllYears::Average, VCardDynamic>(
          survey,
          average[index],
          Category::DataLevel::setOfAreas,
          Category::FileLevel::va,
          precision);

        min[index].buildSurveyReport<IntermediateValues, VCardDynamic>(
          survey,
          values, // not used, placeholder for templates
          Category::DataLevel::setOfAreas,
          Category::FileLevel::va,
          precision);

        max[index].buildSurveyReport<IntermediateValues, VCardDynamic>(
          survey,
          values, // not used, placeholder for templates
          Category::DataLevel::setOfAreas,
          Category::FileLevel::va,
          precision);

        ++index;
    }
}

void SetDataAllYears::appendToSurvey(SurveyResults& survey, Category::Precision precision) const
{
    processGroups(averageThermal_,
                  stdDevThermal_,
                  minThermal_,
                  maxThermal_,
                  thermalGroupNames_,
                  precision,
                  "_TH_PROD",
                  survey);
    processGroups(averageRenewable_,
                  stdDevRenewable_,
                  minRenewable_,
                  maxRenewable_,
                  renewableGroupNames_,
                  precision,
                  "_RES_PROD",
                  survey);
    processGroups(averageStsInjection_,
                  stdDevStsInjection_,
                  minStsInjection_,
                  maxStsInjection_,
                  stsGroupNames_,
                  precision,
                  "_INJECTION",
                  survey);
    processGroups(averageStsWithdrawal_,
                  stdDevStsWithdrawal_,
                  minStsWithdrawal_,
                  maxStsWithdrawal_,
                  stsGroupNames_,
                  precision,
                  "_WITHDRAWAL",
                  survey);

    processGroups(averageStsWithdrawal_,
                  stdDevStsWithdrawal_,
                  minStsWithdrawal_,
                  maxStsWithdrawal_,
                  stsGroupNames_,
                  precision,
                  "_LEVEL",
                  survey);
}
} // namespace Antares::Solver::Variable
