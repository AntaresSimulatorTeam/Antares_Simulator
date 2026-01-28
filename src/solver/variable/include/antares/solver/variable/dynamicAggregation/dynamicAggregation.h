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

#include <filesystem>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/storage/average.h"
#include "antares/solver/variable/storage/minmax.h"
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

    // used for survey results
    struct VCardDynamic
    {
        static constexpr uint8_t decimal = 0;
        static constexpr uint8_t categoryFileLevel = Category::FileLevel::va;
    };
};

class SetDataSingleYear: public SetDataBase
{
public:
    friend class SetDataAllYears;

    explicit SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set);

    void addResultsToSet(const PROBLEME_HEBDO& pb);

    void writeResultsToFolder(const std::filesystem::path& folder,
                              Data::Study& study,
                              IResultWriter& writer) const;

private:
    void processGroup(const std::vector<std::vector<long double>>& results,
                      const std::set<std::string>& groupNames,
                      const Category::Precision& precision,
                      const std::string& suffix,
                      Data::Study& study,
                      SurveyResults& survey,
                      bool doWeAverage = false) const; // average for level

    void processAndSave(Category::Precision precision,
                        const std::string& filename,
                        Data::Study& study,
                        SurveyResults& survey) const;

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

    void writeResultsToFolder(const std::filesystem::path& folder,
                              Data::Study& study,
                              IResultWriter& writer) const;

private:
    void processGroup(const std::vector<R::AllYears::Average<>>& average,
                      const std::vector<R::AllYears::StdDeviation<>>& stdDev,
                      const std::vector<R::AllYears::MinMaxBase<true>>& min,
                      const std::vector<R::AllYears::MinMaxBase<false>>& max,
                      const std::set<std::string>& groupNames,
                      const Category::Precision& precision,
                      const std::string& suffix,
                      SurveyResults& survey) const;

    void processAndSave(Category::Precision precision,
                        const std::string& filename,
                        SurveyResults& survey) const;

    std::vector<R::AllYears::MinMaxBase<true>> minThermal;
    std::vector<R::AllYears::MinMaxBase<false>> maxThermal;

    std::vector<R::AllYears::MinMaxBase<true>> minRenewable;
    std::vector<R::AllYears::MinMaxBase<false>> maxRenewable;

    std::vector<R::AllYears::MinMaxBase<true>> minStsInjection;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsInjection;

    std::vector<R::AllYears::MinMaxBase<true>> minStsWithdrawal;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsWithdrawal;

    std::vector<R::AllYears::MinMaxBase<true>> minStsLevel;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsLevel;

    std::vector<R::AllYears::Average<>> averageThermal;
    std::vector<R::AllYears::Average<>> averageRenewable;
    std::vector<R::AllYears::Average<>> averageStsInjection;
    std::vector<R::AllYears::Average<>> averageStsWithdrawal;
    std::vector<R::AllYears::Average<>> averageStsLevel;

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

    void writeAllResults(const std::filesystem::path& folder, IResultWriter& writer) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataSingleYear> setsData_;
};

class DynamicAggregationAllYears
{
public:
    explicit DynamicAggregationAllYears(Data::Study& study);

    void merge(const DynamicAggregationSingleYear& toMerge, unsigned year);

    void writeAllResults(const std::filesystem::path& folder, IResultWriter& writer) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataAllYears> setsData_;
};

} // namespace Antares::Solver::Variable
