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

#include "antares/solver/variable/dynamic-aggregation/dynamic-aggregation.h"

// TODO  remove tests purpose
#include <filesystem>
#include <fstream>
#include <vector>

namespace Antares::Solver::Variable
{

SetData::SetData(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    // Thermal clusters
    for (auto& area: set_)
    {
        // handle sts later
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            const std::string groupName = cluster->getGroup();
            thermalNameAndNumberOfOccurrences_[groupName]++;
        }
        // Renewable clusters
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            const std::string groupName = cluster->getGroup();
            renewableNameAndNumberOfOccurrences_[groupName]++;
        }
        // STS clusters
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            const std::string& groupName = sts.properties.groupName;
            stsNameAndNumberOfOccurrences_[groupName]++;
        }
    }

    for (const auto& nameAndNum: thermalNameAndNumberOfOccurrences_)
    {
        thermalGroupNames_.push_back(nameAndNum.first);
    }
    thermalGroupToNumbers_ = Utils::giveNumbersToStrings(thermalGroupNames_);

    for (const auto& nameAndNum: renewableNameAndNumberOfOccurrences_)
    {
        renewableGroupNames_.push_back(nameAndNum.first);
    }
    renewableGroupToNumbers_ = Utils::giveNumbersToStrings(renewableGroupNames_);

    for (const auto& nameAndNum: stsNameAndNumberOfOccurrences_)
    {
        stsGroupNames_.push_back(nameAndNum.first);
    }
    stsGroupToNumbers_ = Utils::giveNumbersToStrings(stsGroupNames_);

    for (unsigned int i = 0; i < thermalGroupNames_.size(); i++)
    {
        thermalResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
    for (unsigned int i = 0; i < renewableGroupNames_.size(); i++)
    {
        renewableResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
    for (unsigned int i = 0; i < stsGroupNames_.size(); i++)
    {
        stsInjectionResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
        stsWithdrawalResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
        stsLevelResults_.push_back(std::vector<long double>(HOURS_PER_YEAR, 0));
    }
}

void SetData::addResultsToSet(const PROBLEME_HEBDO& pb, const Data::Study& study)
{
    for (const auto* area: set_)
    {
        // Thermal clusters
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            const std::string& groupName = cluster->getGroup();

            double ratio = 1 / (double)thermalNameAndNumberOfOccurrences_[groupName];
            unsigned index = thermalGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                thermalResults_[index][realHour] += pb.ResultatsHoraires[area->index]
                                                      .ProductionThermique[h]
                                                      .ProductionThermiqueDuPalier[cluster->index]
                                                    * ratio;
            }
        }
        // Renewable clusters
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();
            double ratio = 1 / (double)renewableNameAndNumberOfOccurrences_[groupName];
            unsigned index = renewableGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                // Extract renewable value for this cluster, area, and hour
                // This assumes cluster->valueAtTimeStep is available and correct
                renewableResults_[index][realHour] += cluster->valueAtTimeStep(pb.year, realHour)
                                                      * ratio;
            }
        }
        // STS clusters
        uint clusterIndex = 0;
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            const std::string& groupName = sts.properties.groupName;
            double ratio = 1 / (double)stsNameAndNumberOfOccurrences_[groupName];
            unsigned index = stsGroupToNumbers_[groupName];

            const auto& stsResults = pb.ResultatsHoraires[area->index]
                                       .ShortTermStorage[clusterIndex];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                stsInjectionResults_[index][realHour] += stsResults.injection[h] * ratio;
                stsWithdrawalResults_[index][realHour] += stsResults.withdrawal[h] * ratio;
                stsLevelResults_[index][realHour] += stsResults.level[h] * ratio;
            }
            ++clusterIndex;
        }
    }
}

DynamicAggregation::DynamicAggregation(Data::Study& study):
    study_(study)
{
}

void DynamicAggregation::initializeSetsData()
{
    for (const auto& set: study_.setsOfAreas)
    {
        setsData_.try_emplace(set.first, *set.second);
    }
}

void DynamicAggregation::addResultsToSets(const PROBLEME_HEBDO& pb)
{
    for (auto& [_, setData]: setsData_)
    {
        setData.addResultsToSet(pb, study_);
    }
}

// TODO  remove tests purpose
void DynamicAggregation::writeAllResults(const std::string& baseFolder) const
{
    namespace fs = std::filesystem;
    fs::create_directories(baseFolder);

    for (const auto& [setName, setData]: setsData_)
    {
        std::string folderPath = baseFolder + "/" + setName;
        setData.writeResultsToFolder(folderPath);
    }
}

// TODO  remove tests purpose
void SetData::writeResultsToFolder(const std::string& folderName) const
{
    namespace fs = std::filesystem;
    fs::create_directories(folderName);

    for (size_t i = 0; i < thermalGroupNames_.size(); ++i)
    {
        const std::string& group = thermalGroupNames_[i];
        std::string filePath = folderName + "/" + group + ".txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (size_t h = 0; h < thermalResults_[i].size(); ++h)
            {
                outFile << thermalResults_[i][h] << std::endl;
            }
            outFile.close();
        }

        filePath = folderName + "/" + group + "_annual.txt";
        std::ofstream annualFile(filePath);
        if (annualFile.is_open())
        {
            // For demonstration, sum all values as "annual"
            long double annual = 0.0;
            for (auto v: thermalResults_[i])
            {
                annual += v;
            }
            annualFile << annual << std::endl;
        }
    }
    // Write renewable results
    for (size_t i = 0; i < renewableGroupNames_.size(); ++i)
    {
        std::string fileName = folderName + "/" + renewableGroupNames_[i] + "_renewable.csv";
        std::ofstream out(fileName);
        for (size_t h = 0; h < renewableResults_[i].size(); ++h)
        {
            out << renewableResults_[i][h] << "\n";
        }
        out.close();
    }
    // Write STS results
    for (size_t i = 0; i < stsGroupNames_.size(); ++i)
    {
        {
            std::string fileName = folderName + "/" + stsGroupNames_[i] + "_sts_injection.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsInjectionResults_[i].size(); ++h)
            {
                out << stsInjectionResults_[i][h] << "\n";
            }
            out.close();
        }
        {
            std::string fileName = folderName + "/" + stsGroupNames_[i] + "_sts_withdrawal.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsWithdrawalResults_[i].size(); ++h)
            {
                out << stsWithdrawalResults_[i][h] << "\n";
            }
            out.close();
        }
        {
            std::string fileName = folderName + "/" + stsGroupNames_[i] + "_sts_level.csv";
            std::ofstream out(fileName);
            for (size_t h = 0; h < stsLevelResults_[i].size(); ++h)
            {
                out << stsLevelResults_[i][h] << "\n";
            }
            out.close();
        }
    }
}

} // namespace Antares::Solver::Variable
