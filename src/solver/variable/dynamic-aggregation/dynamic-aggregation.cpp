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

#include <sstream>
#include <vector>

#include "antares/utils/utils.h"
#include "antares/writer/i_writer.h"

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

void DynamicAggregation::writeAllResults(Solver::IResultWriter& writer,
                                         const std::string& baseFolder) const
{
    for (const auto& [setName, setData]: setsData_)
    {
        setData.writeResultsToFolder(writer, baseFolder + "/" + setName);
    }
}

void SetData::writeResultsToFolder(Solver::IResultWriter& writer,
                                   const std::string& folderName) const
{
    for (size_t i = 0; i < thermalGroupNames_.size(); ++i)
    {
        const std::string& group = thermalGroupNames_[i];
        std::string thermalHourlyPath = group + ".txt";

        std::ostringstream thermalHourlyContent;
        for (size_t h = 0; h < thermalResults_[i].size(); ++h)
        {
            thermalHourlyContent << thermalResults_[i][h] << "\n";
        }
        std::string thermalHourlyStr = thermalHourlyContent.str();
        writer.addEntryFromBuffer(thermalHourlyPath, thermalHourlyStr);

        std::string thermalAnnualPath = group + "_annual.txt";
        long double annual = 0.0;
        for (auto v: thermalResults_[i])
        {
            annual += v;
        }
        std::ostringstream thermalAnnualContent;
        thermalAnnualContent << annual;
        std::string thermalAnnualStr = thermalAnnualContent.str();
        writer.addEntryFromBuffer(thermalAnnualPath, thermalAnnualStr);
    }

    for (size_t i = 0; i < renewableGroupNames_.size(); ++i)
    {
        std::string renewablePath = renewableGroupNames_[i] + "_renewable.csv";
        size_t idx = thermalGroupNames_.size() + i;
        std::ostringstream renewableContent;
        for (size_t h = 0; h < renewableResults_[idx].size(); ++h)
        {
            renewableContent << renewableResults_[idx][h] << "\n";
        }
        std::string renewableStr = renewableContent.str();
        writer.addEntryFromBuffer(renewablePath, renewableStr);
    }
    // Write STS results
    auto writeStsResults =
      [&](const std::string& suffix, const std::vector<std::vector<long double>>& results)
    {
        for (size_t i = 0; i < stsGroupNames_.size(); ++i)
        {
            std::string fileName = folderName + "/" + stsGroupNames_[i] + suffix;
            std::ostringstream content;
            for (size_t h = 0; h < results[i].size(); ++h)
            {
                content << results[i][h] << "\n";
            }
            std::string contentStr = content.str();
            writer.addEntryFromBuffer(fileName, contentStr);
        }
    };
    writeStsResults("_sts_injection.csv", stsInjectionResults_);
    writeStsResults("_sts_withdrawal.csv", stsWithdrawalResults_);
    writeStsResults("_sts_level.csv", stsLevelResults_);
}
} // namespace Antares::Solver::Variable
