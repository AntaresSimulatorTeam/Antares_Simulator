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
    minThermal.resize(thermalGroupNames_.size());
    maxThermal.resize(thermalGroupNames_.size());
    minRenewable.resize(renewableGroupNames_.size());
    maxRenewable.resize(renewableGroupNames_.size());
    minStsInjection.resize(stsGroupNames_.size());
    maxStsInjection.resize(stsGroupNames_.size());
    minStsWithdrawal.resize(stsGroupNames_.size());
    maxStsWithdrawal.resize(stsGroupNames_.size());
    minStsLevel.resize(stsGroupNames_.size());
    maxStsLevel.resize(stsGroupNames_.size());

    averageThermal.resize(thermalGroupNames_.size());
    averageRenewable.resize(renewableGroupNames_.size());
    averageStsInjection.resize(stsGroupNames_.size());
    averageStsWithdrawal.resize(stsGroupNames_.size());
    averageStsLevel.resize(stsGroupNames_.size());

    stdDevThermal.resize(thermalGroupNames_.size());
    stdDevRenewable.resize(renewableGroupNames_.size());
    stdDevStsInjection.resize(stsGroupNames_.size());
    stdDevStsWithdrawal.resize(stsGroupNames_.size());
    stdDevStsLevel.resize(stsGroupNames_.size());

    for (size_t i = 0; i < thermalGroupNames_.size(); ++i)
    {
        minThermal[i].resetInf();
        maxThermal[i].resetSup();
        averageThermal[i].initializeFromStudy(study);
        averageThermal[i].reset();
        stdDevThermal[i].initializeFromStudy(study);
        stdDevThermal[i].reset();
    }
    for (size_t i = 0; i < renewableGroupNames_.size(); ++i)
    {
        minRenewable[i].resetInf();
        maxRenewable[i].resetSup();
        averageRenewable[i].initializeFromStudy(study);
        averageRenewable[i].reset();
        stdDevRenewable[i].initializeFromStudy(study);
        stdDevRenewable[i].reset();
    }
    for (size_t i = 0; i < stsGroupNames_.size(); ++i)
    {
        minStsInjection[i].resetInf();
        maxStsInjection[i].resetSup();
        minStsWithdrawal[i].resetInf();
        maxStsWithdrawal[i].resetSup();
        minStsLevel[i].resetInf();
        maxStsLevel[i].resetSup();
        averageStsInjection[i].initializeFromStudy(study);
        averageStsInjection[i].reset();
        averageStsWithdrawal[i].initializeFromStudy(study);
        averageStsWithdrawal[i].reset();
        averageStsLevel[i].initializeFromStudy(study);
        averageStsLevel[i].reset();
        stdDevStsInjection[i].initializeFromStudy(study);
        stdDevStsInjection[i].reset();
        stdDevStsWithdrawal[i].initializeFromStudy(study);
        stdDevStsWithdrawal[i].reset();
        stdDevStsLevel[i].initializeFromStudy(study);
        stdDevStsLevel[i].reset();
    }
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
        minThermal[i].mergeInf(year, values);
        maxThermal[i].mergeSup(year, values);
        averageThermal[i].merge(year, values);
        stdDevThermal[i].merge(year, values);
    }

    // Renewable
    for (size_t i = 0; i < toMerge.renewableResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.renewableResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minRenewable[i].mergeInf(year, values);
        maxRenewable[i].mergeSup(year, values);
        averageRenewable[i].merge(year, values);
        stdDevRenewable[i].merge(year, values);
    }

    // STS Injection
    for (size_t i = 0; i < toMerge.stsInjectionResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.stsInjectionResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsInjection[i].mergeInf(year, values);
        maxStsInjection[i].mergeSup(year, values);
        averageStsInjection[i].merge(year, values);
        stdDevStsInjection[i].merge(year, values);

        std::ranges::copy(toMerge.stsWithdrawalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsWithdrawal[i].mergeInf(year, values);
        maxStsWithdrawal[i].mergeSup(year, values);
        averageStsWithdrawal[i].merge(year, values);
        stdDevStsWithdrawal[i].merge(year, values);

        std::ranges::copy(toMerge.stsLevelResults_[i], values.hour);
        values.computeAveragesForCurrentYearFromHourlyResults();
        minStsLevel[i].mergeInf(year, values);
        maxStsLevel[i].mergeSup(year, values);
        averageStsLevel[i].merge(year, values);
        stdDevStsLevel[i].merge(year, values);
    }
}

struct VCardStub
{
    enum
    {
        decimal = 0
    };
};

void SetDataAllYears::processGroup(const auto& avgVec,
                                   const auto& stdVec,
                                   const auto& minVec,
                                   const auto& maxVec,
                                   const std::set<std::string>& groupNames,
                                   const std::string& suffix,
                                   Data::Study& study,
                                   SurveyResults& survey) const
{
    size_t index = 0;

    auto setSurvey = [&](const std::string& group, const std::string& type)
    {
        survey.captions[0][survey.data.columnIndex] = group + suffix;
        survey.captions[1][survey.data.columnIndex] = "MWh";
        survey.captions[2][survey.data.columnIndex] = type;
        survey.variableCaption = group + suffix;
        survey.variableUnit = "MWh";
    };

    for (const auto& group: groupNames)
    {
        IntermediateValues values;
        values.initializeFromStudy(study);

        // exp
        setSurvey(group, "exp");
        values.reset();
        std::ranges::copy(avgVec[index].hourly, values.hour);
        values.computeStatisticsForTheCurrentYear();
        values.buildAnnualSurveyReport<VCardStub>(survey,
                                                  Category::FileLevel::va,
                                                  Category::Precision::hourly);

        // std
        setSurvey(group, "std");
        values.reset();
        std::ranges::copy(stdVec[index].stdDeviationHourly, values.hour);
        values.computeStatisticsForTheCurrentYear();
        values.buildAnnualSurveyReport<VCardStub>(survey,
                                                  Category::FileLevel::va,
                                                  Category::Precision::hourly);

        // min
        setSurvey(group, "min");
        values.reset();
        for (size_t h = 0; h < HOURS_PER_YEAR; ++h)
        {
            values.hour[h] = minVec[index].hourly[h].value;
        }
        values.computeStatisticsForTheCurrentYear();
        values.buildAnnualSurveyReport<VCardStub>(survey,
                                                  Category::FileLevel::va,
                                                  Category::Precision::hourly);

        // max
        setSurvey(group, "max");
        values.reset();
        for (size_t h = 0; h < HOURS_PER_YEAR; ++h)
        {
            values.hour[h] = maxVec[index].hourly[h].value;
        }
        values.computeStatisticsForTheCurrentYear();
        values.buildAnnualSurveyReport<VCardStub>(survey,
                                                  Category::FileLevel::va,
                                                  Category::Precision::hourly);

        ++index;
    }
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

    survey.data.columnIndex = 0;

    processGroup(averageThermal,
                 stdDevThermal,
                 minThermal,
                 maxThermal,
                 thermalGroupNames_,
                 "",
                 study,
                 survey);
    processGroup(averageRenewable,
                 stdDevRenewable,
                 minRenewable,
                 maxRenewable,
                 renewableGroupNames_,
                 "",
                 study,
                 survey);
    processGroup(averageStsInjection,
                 stdDevStsInjection,
                 minStsInjection,
                 maxStsInjection,
                 stsGroupNames_,
                 "_INJECTION",
                 study,
                 survey);
    processGroup(averageStsWithdrawal,
                 stdDevStsWithdrawal,
                 minStsWithdrawal,
                 maxStsWithdrawal,
                 stsGroupNames_,
                 "_WITHDRAWAL",
                 study,
                 survey);

    // Save the results
    survey.data.filename = (folder / "hourly.txt").string();
    survey.saveToFile(Category::DataLevel::setOfAreas,
                      Category::FileLevel::va,
                      Category::Precision::hourly);
}

} // namespace Antares::Solver::Variable
