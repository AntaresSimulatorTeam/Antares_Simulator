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

#pragma once

#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/storage/averagedata.h"
#include "antares/solver/variable/storage/minmax-data.h"
#include "antares/solver/variable/storage/stdDeviation.h"

namespace Antares::Solver::Variable
{

class SetDataBase
{
public:
    explicit SetDataBase(const std::set<Data::Area*, Data::CompareAreaName>& set);

protected:
    std::set<std::string> thermalGroupNames_;
    std::map<std::string, unsigned int> thermalGroupToNumbers_;

    std::set<std::string> renewableGroupNames_;
    std::map<std::string, unsigned int> renewableGroupToNumbers_;

    std::set<std::string> stsGroupNames_;
    std::map<std::string, unsigned int> stsGroupToNumbers_;
    const std::set<Data::Area*, Data::CompareAreaName>& set_;
};

class SetDataSingleYear: public SetDataBase
{
public:
    friend class SetDataAllYears;

    explicit SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set);

    void addResultsToSet(const PROBLEME_HEBDO& pb);

    // TODO  remove tests purpose
    void writeResultsToFolder(const std::string& folderName) const;

private:
    std::vector<std::vector<HighPrecision>> thermalResults_;
    std::vector<std::vector<HighPrecision>> renewableResults_;
    std::vector<std::vector<HighPrecision>> stsInjectionResults_;
    std::vector<std::vector<HighPrecision>> stsWithdrawalResults_;
    std::vector<std::vector<HighPrecision>> stsLevelResults_;
};

class SetDataAllYears: public SetDataBase
{
public:
    explicit SetDataAllYears(const std::set<Data::Area*, Data::CompareAreaName>& set,
                             Data::Study& study);

    void merge(const SetDataSingleYear& toMerge, Data::Study& study, unsigned year);

    // TODO  remove tests purpose
    void writeResultsToFolder(const std::string& folderName,
                              Data::Study& study,
                              IResultWriter& writer) const;

private:
    std::vector<R::AllYears::MinMaxData> minThermal;
    std::vector<R::AllYears::MinMaxData> maxThermal;

    std::vector<R::AllYears::MinMaxData> minRenewable;
    std::vector<R::AllYears::MinMaxData> maxRenewable;

    std::vector<R::AllYears::MinMaxData> minStsInjection;
    std::vector<R::AllYears::MinMaxData> maxStsInjection;

    std::vector<R::AllYears::MinMaxData> minStsWithdrawal;
    std::vector<R::AllYears::MinMaxData> maxStsWithdrawal;

    std::vector<R::AllYears::MinMaxData> minStsLevel;
    std::vector<R::AllYears::MinMaxData> maxStsLevel;

    std::vector<R::AllYears::AverageData> averageThermal;
    std::vector<R::AllYears::AverageData> averageRenewable;
    std::vector<R::AllYears::AverageData> averageStsInjection;
    std::vector<R::AllYears::AverageData> averageStsWithdrawal;
    std::vector<R::AllYears::AverageData> averageStsLevel;

    std::vector<R::AllYears::StdDeviation<>> stdDevThermal;
    std::vector<R::AllYears::StdDeviation<>> stdDevRenewable;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsInjection;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsWithdrawal;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsLevel;
};

class DynamicAggregationSingleYear
{
public:
    friend class DynamicAggregationAllYears;

    explicit DynamicAggregationSingleYear(Data::Study& study);

    void addResultsToSets(const PROBLEME_HEBDO& pb);

    // TODO rm tests
    void writeAllResults(const std::string& baseFolder) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataSingleYear> setsData_;
};

class DynamicAggregationAllYears
{
public:
    explicit DynamicAggregationAllYears(Data::Study& study);

    void merge(const DynamicAggregationSingleYear& toMerge, unsigned year);

    // TODO rm tests
    void writeAllResults(const std::string& baseFolder, IResultWriter& writer) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataAllYears> setsData_;
};

} // namespace Antares::Solver::Variable
