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

#include <fstream>

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

namespace Antares::Solver::Variable
{

SetDataSingleYear::SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set):
    SetDataBase(set)
{
    for (unsigned int i = 0; i < thermalGroupNames_.size(); ++i)
    {
        thermalResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
    for (unsigned int i = 0; i < renewableGroupNames_.size(); ++i)
    {
        renewableResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
    for (unsigned int i = 0; i < stsGroupNames_.size(); ++i)
    {
        stsInjectionResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
        stsWithdrawalResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
        stsLevelResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
}

void SetDataSingleYear::addResultsToSet(const PROBLEME_HEBDO& pb)
{
    for (const auto* area: set_)
    {
        // Thermal clusters
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            const std::string& groupName = cluster->getGroup();
            unsigned index = thermalGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                thermalResults_[index][realHour] += pb.ResultatsHoraires[area->index]
                                                      .ProductionThermique[h]
                                                      .ProductionThermiqueDuPalier[cluster->index];
            }
        }
        // Renewable clusters
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();
            unsigned index = renewableGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                renewableResults_[index][realHour] += cluster->valueAtTimeStep(pb.year, realHour);
            }
        }
        // STS clusters
        uint clusterIndex = 0;
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            const std::string& groupName = sts.properties.groupName;
            unsigned index = stsGroupToNumbers_[groupName];

            const auto& stsResults = pb.ResultatsHoraires[area->index]
                                       .ShortTermStorage[clusterIndex];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                stsInjectionResults_[index][realHour] += stsResults.injection[h];
                stsWithdrawalResults_[index][realHour] += stsResults.withdrawal[h];
                stsLevelResults_[index][realHour] += stsResults.level[h];
            }
            ++clusterIndex;
        }
    }
}

// TODO  remove tests purpose
void SetDataSingleYear::writeResultsToFolder(const std::string& folderName,
                                             Data::Study& study,
                                             IResultWriter& writer) const
{
    unsigned int nbVariables = thermalGroupNames_.size() + renewableGroupNames_.size()
                               + stsGroupNames_.size() * 3;

    SurveyResults survey(study, nbVariables, "out", writer);

    bool nonApplicable[2] = {false, false};
    bool printed[2] = {true, true};

    survey.isCurrentVarNA = nonApplicable;
    survey.isPrinted = printed;

    struct VCardStub
    {
        enum
        {
            decimal = 0,
        };
    };

    survey.data.columnIndex = 0;

    auto processGroup = [&](const std::vector<std::vector<long double>>& results,
                            const std::set<std::string>& groupNames,
                            const Category::Precision& precision,
                            const std::string& suffix = "") {
        unsigned int index = 0;
        for (const auto& group: groupNames)
        {
            survey.captions[0][survey.data.columnIndex] = group + suffix;
            survey.captions[1][survey.data.columnIndex] = "MWh";
            survey.captions[2][survey.data.columnIndex] = "";
            survey.variableCaption = group + suffix;
            survey.variableUnit = "MWh";

            IntermediateValues values;
            values.initializeFromStudy(study);
            values.reset();
            std::ranges::copy(results[index], values.hour);
            values.computeStatisticsForTheCurrentYear();

            values.buildAnnualSurveyReport<VCardStub>(survey,
                                                      Category::FileLevel::va,
                                                      precision);
            ++index;
        }
    };

    auto processWithPrecision = [&](Category::Precision precision) {
        survey.data.columnIndex = 0;
        processGroup(thermalResults_, thermalGroupNames_, precision);
        processGroup(renewableResults_, renewableGroupNames_, precision);
        processGroup(stsInjectionResults_, stsGroupNames_, precision, "_INJECTION");
        processGroup(stsWithdrawalResults_, stsGroupNames_, precision, "_WITHDRAWAL");
        processGroup(stsLevelResults_, stsGroupNames_, precision, "_LEVEL");
    };

    auto processAndSave = [&](Category::Precision precision, const std::string& filename) {
        processWithPrecision(precision);
        survey.data.filename = filename;
        survey.saveToFile(Category::DataLevel::setOfAreas,
                          Category::FileLevel::va,
                          precision);
    };

    processAndSave(Category::Precision::hourly, "hourly.txt");
    processAndSave(Category::Precision::daily, "daily.txt");
    processAndSave(Category::Precision::weekly, "weekly.txt");
    processAndSave(Category::Precision::monthly, "monthly.txt");
    processAndSave(Category::Precision::annual, "annual.txt");
    survey.data.columnIndex = 0;
}

} // namespace Antares::Solver::Variable
