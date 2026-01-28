/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

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
    initAndReset(averageThermal, stdDevThermal, minThermal, maxThermal, thermalGroupNames_.size());

    // Renewable
    initAndReset(averageRenewable,
                 stdDevRenewable,
                 minRenewable,
                 maxRenewable,
                 renewableGroupNames_.size());

    // STS
    initAndReset(averageStsInjection,
                 stdDevStsInjection,
                 minStsInjection,
                 maxStsInjection,
                 stsGroupNames_.size());
    initAndReset(averageStsWithdrawal,
                 stdDevStsWithdrawal,
                 minStsWithdrawal,
                 maxStsWithdrawal,
                 stsGroupNames_.size());
    initAndReset(averageStsLevel, stdDevStsLevel, minStsLevel, maxStsLevel, stsGroupNames_.size());
}

void SetDataAllYears::merge(const SetDataSingleYear& toMerge, Data::Study& study, unsigned year)
{
    IntermediateValues values;
    values.initializeFromStudy(study);

    // Thermal
    for (size_t i = 0; i < toMerge.thermalResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.thermalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minThermal[i].merge(year, values);
        maxThermal[i].merge(year, values);
        averageThermal[i].merge(year, values);
        stdDevThermal[i].merge(year, values);
    }

    // Renewable
    for (size_t i = 0; i < toMerge.renewableResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.renewableResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minRenewable[i].merge(year, values);
        maxRenewable[i].merge(year, values);
        averageRenewable[i].merge(year, values);
        stdDevRenewable[i].merge(year, values);
    }

    // STS Injection
    for (size_t i = 0; i < toMerge.stsInjectionResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.stsInjectionResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsInjection[i].merge(year, values);
        maxStsInjection[i].merge(year, values);
        averageStsInjection[i].merge(year, values);
        stdDevStsInjection[i].merge(year, values);

        std::ranges::copy(toMerge.stsWithdrawalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsWithdrawal[i].merge(year, values);
        maxStsWithdrawal[i].merge(year, values);
        averageStsWithdrawal[i].merge(year, values);
        stdDevStsWithdrawal[i].merge(year, values);

        std::ranges::copy(toMerge.stsLevelResults_[i], values.hour);
        values.computeAveragesForCurrentYearFromHourlyResults();
        minStsLevel[i].merge(year, values);
        maxStsLevel[i].merge(year, values);
        averageStsLevel[i].merge(year, values);
        stdDevStsLevel[i].merge(year, values);
    }
}

void SetDataAllYears::processGroup(const std::vector<R::AllYears::Average<>>& average,
                                   const std::vector<R::AllYears::StdDeviation<>>& stdDev,
                                   const std::vector<R::AllYears::MinMaxBase<true>>& min,
                                   const std::vector<R::AllYears::MinMaxBase<false>>& max,
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

        stdDev[index].buildSurveyReport<R::AllYears::Average<>, VCardDynamic>(
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

void SetDataAllYears::processAndSave(Category::Precision precision,
                                     const std::string& filename,
                                     SurveyResults& survey) const
{
    survey.data.columnIndex = 0;

    processGroup(averageThermal,
                 stdDevThermal,
                 minThermal,
                 maxThermal,
                 thermalGroupNames_,
                 precision,
                 "",
                 survey);
    processGroup(averageRenewable,
                 stdDevRenewable,
                 minRenewable,
                 maxRenewable,
                 renewableGroupNames_,
                 precision,
                 "",
                 survey);
    processGroup(averageStsInjection,
                 stdDevStsInjection,
                 minStsInjection,
                 maxStsInjection,
                 stsGroupNames_,
                 precision,
                 "_INJECTION",
                 survey);
    processGroup(averageStsWithdrawal,
                 stdDevStsWithdrawal,
                 minStsWithdrawal,
                 maxStsWithdrawal,
                 stsGroupNames_,
                 precision,
                 "_WITHDRAWAL",
                 survey);

    processGroup(averageStsWithdrawal,
                 stdDevStsWithdrawal,
                 minStsWithdrawal,
                 maxStsWithdrawal,
                 stsGroupNames_,
                 precision,
                 "_LEVEL",
                 survey);

    survey.data.filename = filename;
    survey.saveToFile(Category::DataLevel::setOfAreas, Category::FileLevel::va, precision);
}

void SetDataAllYears::writeResultsToFolder(const std::filesystem::path& folder,
                                           Data::Study& study,
                                           IResultWriter& writer) const
{
    // Calculate total number of variables (4 columns per group: exp, std, min, max)
    const std::size_t nbVariables = (thermalGroupNames_.size() + renewableGroupNames_.size()
                                     + stsGroupNames_.size() * 3)
                                    * 4;

    SurveyResults survey(study, nbVariables, folder.string(), writer);

    bool nonApplicable[2] = {false, false};
    bool printed[2] = {true, true};

    survey.isCurrentVarNA = nonApplicable;
    survey.isPrinted = printed;

    // Save for all precisions
    processAndSave(Category::Precision::hourly, (folder / "hourly.txt").string(), survey);
    processAndSave(Category::Precision::daily, (folder / "daily.txt").string(), survey);
    processAndSave(Category::Precision::weekly, (folder / "weekly.txt").string(), survey);
    processAndSave(Category::Precision::monthly, (folder / "monthly.txt").string(), survey);
    processAndSave(Category::Precision::annual, (folder / "annual.txt").string(), survey);
}

} // namespace Antares::Solver::Variable
