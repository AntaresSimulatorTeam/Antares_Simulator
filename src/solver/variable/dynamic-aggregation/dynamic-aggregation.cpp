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

namespace Antares::Solver::Variable
{

ValuesStorage::ValuesStorage():
    values_(HOURS_PER_YEAR, 0)
{
}

void ValuesStorage::merge(const double* values, long double ratio)
{
    for (size_t i = 0; i < HOURS_PER_YEAR; ++i)
    {
        values_[i] += values[i] * ratio;
    }
}

const std::vector<long double>& ValuesStorage::data() const
{
    return values_;
}

SetData::SetData(const std::set<Data::Area*, Data::CompareAreaName>& set):
    set_(set)
{
    std::map<std::string, int> nameAndNumberOfOccurrences_;
    for (auto& area: set_)
    {
        // handle sts later
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const std::string groupName = cluster->getGroup();
            nameAndNumberOfOccurrences_[groupName]++;
        }
    }

    for (const auto& nameAndNum: nameAndNumberOfOccurrences_)
    {
        groupNames_.push_back(nameAndNum.first);
    }
    groupToNumbers_ = Utils::giveNumbersToStrings(groupNames_);
    unsigned int nbColumns = groupNames_.size();

    for (unsigned int i = 0; i < nbColumns; i++)
    {
        results_.push_back(ValuesStorage());
    }
}

// TODO  remove tests purpose
void SetData::writeResultsToFolder(const std::string& folderName) const
{
    namespace fs = std::filesystem;
    fs::create_directories(folderName);

    for (size_t i = 0; i < groupNames_.size(); ++i)
    {
        const std::string& group = groupNames_[i];
        std::string filePath = folderName + "/" + group + ".txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (size_t h = 0; h < results_[i].data().size(); ++h)
            {
                outFile << results_[i].data()[h] << std::endl;
            }
            outFile.close();
        }

        filePath = folderName + "/" + group + "_annual.txt";
        std::ofstream annualFile(filePath);
        if (annualFile.is_open())
        {
            // For demonstration, sum all values as "annual"
            long double annual = 0.0;
            for (auto v: results_[i].data())
            {
                annual += v;
            }
            annualFile << annual << std::endl;
        }
    }
}

void SetData::addResultsToSet(State& state, Data::Study& study)
{
    for (const auto* area: set_)
    {
        for (const auto& cluster: area->thermal.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();

            double prod[HOURS_PER_YEAR];
            for (size_t h = 0; h < HOURS_PER_YEAR; ++h)
            {
                /*prod[h] = state.hourlyResults->ProductionThermique[h]*/
                /*            .ProductionThermiqueDuPalier[cluster->index];*/
            }
            double ratio = 1 / (double)nameAndNumberOfOccurrences_[groupName];
            mergeValues(groupName, prod, ratio);
        }
    }
}

void SetData::mergeValues(const std::string& groupName, const double* values, long double ratio)
{
    unsigned index = groupToNumbers_[groupName];
    results_[index].merge(values, ratio);
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

void DynamicAggregation::addResultsToSets(State& state)
{
    for (auto& [_, setData]: setsData_)
    {
        setData.addResultsToSet(state, study_);
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

} // namespace Antares::Solver::Variable
