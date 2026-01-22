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
        stdDevRenewable[i].merge(0, values);
    }

    // STS Injection
    for (size_t i = 0; i < toMerge.stsInjectionResults_.size(); ++i)
    {
        std::ranges::copy(toMerge.stsInjectionResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsInjection[i].mergeInf(0, values);
        maxStsInjection[i].mergeSup(0, values);
        averageStsInjection[i].merge(0, values);
        stdDevStsInjection[i].merge(0, values);

        std::ranges::copy(toMerge.stsWithdrawalResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsWithdrawal[i].mergeInf(0, values);
        maxStsWithdrawal[i].mergeSup(0, values);
        averageStsWithdrawal[i].merge(0, values);
        stdDevStsWithdrawal[i].merge(0, values);

        std::ranges::copy(toMerge.stsLevelResults_[i], values.hour);
        values.computeStatisticsForTheCurrentYear();
        minStsLevel[i].mergeInf(0, values);
        maxStsLevel[i].mergeSup(0, values);
        averageStsLevel[i].merge(0, values);
        stdDevStsLevel[i].merge(0, values);
    }
}

// TODO  remove tests purpose
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
        std::string filePath = folderName + "/" + group + "min_.txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (const auto& min: minRenewable[index].hourly)
            {
                outFile << min.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "max_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& max: maxRenewable[index].hourly)
            {
                outFile << max.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "exp_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& average: averageRenewable[index].hourly)
            {
                outFile << average << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "std_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& std: stdDevRenewable[index].stdDeviationHourly)
            {
                outFile << std << std::endl;
            }
            outFile.close();
        }

        ++index;
    }
    index = 0;
    // Write STS results
    for (const auto& group: stsGroupNames_)
    {
        std::string filePath = folderName + "/" + group + "inj_min_.txt";
        std::ofstream outFile(filePath);
        if (outFile.is_open())
        {
            for (const auto& min: minStsInjection[index].hourly)
            {
                outFile << min.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "inj_max_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& max: maxStsInjection[index].hourly)
            {
                outFile << max.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "inj_exp_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& average: averageStsInjection[index].hourly)
            {
                outFile << average << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "inj_std_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& std: stdDevStsInjection[index].stdDeviationHourly)
            {
                outFile << std << std::endl;
            }
            outFile.close();
        }

        // Withdrawal
        filePath = folderName + "/" + group + "wtd_min_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& min: minStsWithdrawal[index].hourly)
            {
                outFile << min.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "wtd_max_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& max: maxStsWithdrawal[index].hourly)
            {
                outFile << max.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "wtd_exp_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& average: averageStsWithdrawal[index].hourly)
            {
                outFile << average << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "wtd_std_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& std: stdDevStsWithdrawal[index].stdDeviationHourly)
            {
                outFile << std << std::endl;
            }
            outFile.close();
        }

        // Level
        filePath = folderName + "/" + group + "lvl_min_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& min: minStsLevel[index].hourly)
            {
                outFile << min.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "lvl_max_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& max: maxStsLevel[index].hourly)
            {
                outFile << max.value << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "lvl_exp_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& average: averageStsLevel[index].hourly)
            {
                outFile << average << std::endl;
            }
            outFile.close();
        }
        filePath = folderName + "/" + group + "lvl_std_.txt";
        outFile.open(filePath);
        if (outFile.is_open())
        {
            for (const auto& std: stdDevStsLevel[index].stdDeviationHourly)
            {
                outFile << std << std::endl;
            }
            outFile.close();
        }

        ++index;
    }
}

} // namespace Antares::Solver::Variable
