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

// TODO  remove tests purpose
#include <filesystem>
#include <fstream>
#include <vector>

namespace Antares::Solver::Variable
{

SetDataBase::SetDataBase(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    for (auto& area: set_)
    {
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            thermalGroupNames_.insert(cluster->getGroup());
        }
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            renewableGroupNames_.insert(cluster->getGroup());
        }
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            stsGroupNames_.insert(sts.properties.groupName);
        }
    }

    thermalGroupToNumbers_ = Utils::giveNumbersToStrings(thermalGroupNames_);
    renewableGroupToNumbers_ = Utils::giveNumbersToStrings(renewableGroupNames_);
    stsGroupToNumbers_ = Utils::giveNumbersToStrings(stsGroupNames_);
}

SetDataSingleYear::SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set):
    SetDataBase(set)
{
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
    }
}

void SetDataAllYears::merge(const SetDataSingleYear& toMerge, Data::Study& study)
{
    IntermediateValues values;
    values.initializeFromStudy(study);

    // Thermal
    for (size_t i = 0; i < toMerge.thermalResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.thermalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minThermal[i].mergeInf(0, values);
        maxThermal[i].mergeSup(0, values);
        averageThermal[i].merge(0, values);
        stdDevThermal[i].merge(0, values);
    }

    // Renewable
    for (size_t i = 0; i < toMerge.renewableResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.renewableResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minRenewable[i].mergeInf(0, values);
        maxRenewable[i].mergeSup(0, values);
        averageRenewable[i].merge(0, values);
    }

    // STS Injection
    for (size_t i = 0; i < toMerge.stsInjectionResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.stsInjectionResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsInjection[i].mergeInf(0, values);
        maxStsInjection[i].mergeSup(0, values);
        averageStsInjection[i].merge(0, values);

        std::ranges::copy(toMerge.stsWithdrawalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsWithdrawal[i].mergeInf(0, values);
        maxStsWithdrawal[i].mergeSup(0, values);
        averageStsWithdrawal[i].merge(0, values);

        std::ranges::copy(toMerge.stsLevelResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsLevel[i].mergeInf(0, values);
        maxStsLevel[i].mergeSup(0, values);
        averageStsLevel[i].merge(0, values);
    }
}

DynamicAggregationSingleYear::DynamicAggregationSingleYear(Data::Study& study):
    study_(study)
{
    for (const auto& set: study_.setsOfAreas)
    {
        setsData_.try_emplace(set.first, *set.second);
    }
}

void DynamicAggregationSingleYear::addResultsToSets(const PROBLEME_HEBDO& pb)
{
    for (auto& [_, setData]: setsData_)
    {
        setData.addResultsToSet(pb);
    }
}

DynamicAggregationAllYears::DynamicAggregationAllYears(Data::Study& study):
    study_(study)
{
    for (const auto& set: study_.setsOfAreas)
    {
        setsData_.try_emplace(set.first, *set.second, study_);
    }
}

void DynamicAggregationAllYears::merge(const DynamicAggregationSingleYear& toMerge)
{
    for (auto& [setName, setData]: setsData_)
    {
        const auto& toMergeSetDataIt = toMerge.setsData_.find(setName);
        if (toMergeSetDataIt != toMerge.setsData_.end())
        {
            const SetDataSingleYear& toMergeSetData = toMergeSetDataIt->second;
            setData.merge(toMergeSetData, study_);
        }
    }
}

// TODO  remove tests purpose
void DynamicAggregationSingleYear::writeAllResults(const std::string& baseFolder) const
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
void DynamicAggregationAllYears::writeAllResults(const std::string& baseFolder) const
{
    namespace fs = std::filesystem;
    fs::create_directories(baseFolder);

    for (const auto& [setName, setData]: setsData_)
    {
        std::string folderPath = baseFolder + "/" + setName + "_allYears";
        setData.writeResultsToFolder(folderPath);
    }
}

// TODO  remove tests purpose
void SetDataSingleYear::writeResultsToFolder(const std::string& folderName) const
{
    namespace fs = std::filesystem;
    fs::create_directories(folderName);

    unsigned index = 0;
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

void SetDataAllYears::writeResultsToFolder(const std::string& folderName) const
{
    namespace fs = std::filesystem;
    fs::create_directories(folderName);

    unsigned index = 0;
    for (const auto& group: thermalGroupNames_)
    {
        std::string filePath = folderName + "/" + group + "min_.txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (const auto& min: minThermal[index].hourly)
            {
                outFile << min.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "max_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& max: maxThermal[index].hourly)
            {
                outFile << max.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "exp_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& average: averageThermal[index].hourly)
            {
                outFile << average << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "std_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& std: stdDevThermal[index].stdDeviationHourly)
            {
                outFile << std << std::endl;
            }
            outFile.close();
        }

        ++index;
    }

    index = 0;
    // Write renewable results
    for (const auto& group: renewableGroupNames_)
    {
        ++index;
    }
    index = 0;
    // Write STS results
    for (const auto& group: stsGroupNames_)
    {
        ++index;
    }
}

} // namespace Antares::Solver::Variable
