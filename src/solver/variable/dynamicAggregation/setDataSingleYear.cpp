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

    unsigned int index = 0;
    SurveyResults survey(study, nbVariables, "out", writer);
    survey.data.columnIndex = nbVariables;

    for (const auto& group: thermalGroupNames_)
    {
        survey.captions[0][index] = group;
        survey.captions[1][index] = "MWh";
        survey.captions[2][index] = "";

        std::ranges::copy(thermalResults_[index], survey.values[index]);

        ++index;
    }

    const std::string filename = "hourly.txt";
    survey.data.filename = filename;
    survey.saveToFile(Category::DataLevel::setOfAreas,
                      Category::FileLevel::va,
                      Category::Precision::hourly);

    ///////////////// OLD DEBUG
    ///
    namespace fs = std::filesystem;
    fs::create_directories(folderName);

    index = 0;
    for (const auto& group: thermalGroupNames_)
    {
        std::string filePath = folderName + "/" + group + ".txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (size_t h = 0; h < thermalResults_[index].size(); ++h)
            {
                outFile << thermalResults_[index][h] << std::endl;
            }
            outFile.close();
        }

        filePath = folderName + "/" + group + "_annual.txt";
        std::ofstream annualFile(filePath);
        if (annualFile.is_open())
        {
            // For demonstration, sum all values as "annual"
            long double annual = 0.0;
            for (auto v: thermalResults_[index])
            {
                annual += v;
            }
            annualFile << annual << std::endl;
        }
        ++index;
    }

    index = 0;
    // Write renewable results
    for (const auto& group: renewableGroupNames_)
    {
        std::string fileName = folderName + "/" + group + "_renewable.csv";
        std::ofstream out(fileName);
        for (size_t h = 0; h < renewableResults_[index].size(); ++h)
        {
            out << renewableResults_[index][h] << "\n";
        }
        out.close();
        ++index;
    }
    index = 0;
    // Write STS results
    for (const auto& group: stsGroupNames_)
    {
        {
            std::string fileName = folderName + "/" + group + "_sts_injection.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsInjectionResults_[index].size(); ++h)
            {
                out << stsInjectionResults_[index][h] << "\n";
            }
            out.close();
        }
        {
            std::string fileName = folderName + "/" + group + "_sts_withdrawal.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsWithdrawalResults_[index].size(); ++h)
            {
                out << stsWithdrawalResults_[index][h] << "\n";
            }
            out.close();
        }
        {
            std::string fileName = folderName + "/" + group + "_sts_level.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsLevelResults_[index].size(); ++h)
            {
                out << stsLevelResults_[index][h] << "\n";
            }
            out.close();
        }
        ++index;
    }
}

} // namespace Antares::Solver::Variable
