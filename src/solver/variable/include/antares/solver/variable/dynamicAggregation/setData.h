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
#include <set>
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

    virtual size_t numberOfVariables() const = 0; // 4 variables for mc-all: exp std min max

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

    size_t numberOfVariables() const override;

    void appendToSurvey(SurveyResults& survey,
                        Category::Precision precision,
                        Data::Study& study) const;

private:
    void processGroups(const std::vector<std::vector<long double>>& results,
                       const std::set<std::string>& groupNames,
                       const Category::Precision& precision,
                       const std::string& suffix,
                       Data::Study& study,
                       SurveyResults& survey,
                       bool doWeAverage = false) const; // average for level

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

    size_t numberOfVariables() const override;

    void appendToSurvey(SurveyResults& survey, Category::Precision precision) const;

private:
    void processGroups(const std::vector<R::AllYears::Average<>>& average,
                       const std::vector<R::AllYears::StdDeviation<>>& stdDev,
                       const std::vector<R::AllYears::MinMaxBase<true>>& min,
                       const std::vector<R::AllYears::MinMaxBase<false>>& max,
                       const std::set<std::string>& groupNames,
                       const Category::Precision& precision,
                       const std::string& suffix,
                       SurveyResults& survey) const;

    std::vector<R::AllYears::MinMaxBase<true>> minThermal_;
    std::vector<R::AllYears::MinMaxBase<false>> maxThermal_;

    std::vector<R::AllYears::MinMaxBase<true>> minRenewable_;
    std::vector<R::AllYears::MinMaxBase<false>> maxRenewable_;

    std::vector<R::AllYears::MinMaxBase<true>> minStsInjection_;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsInjection_;

    std::vector<R::AllYears::MinMaxBase<true>> minStsWithdrawal_;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsWithdrawal_;

    std::vector<R::AllYears::MinMaxBase<true>> minStsLevel_;
    std::vector<R::AllYears::MinMaxBase<false>> maxStsLevel_;

    std::vector<R::AllYears::Average<>> averageThermal_;
    std::vector<R::AllYears::Average<>> averageRenewable_;
    std::vector<R::AllYears::Average<>> averageStsInjection_;
    std::vector<R::AllYears::Average<>> averageStsWithdrawal_;
    std::vector<R::AllYears::Average<>> averageStsLevel_;

    std::vector<R::AllYears::StdDeviation<>> stdDevThermal_;
    std::vector<R::AllYears::StdDeviation<>> stdDevRenewable_;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsInjection_;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsWithdrawal_;
    std::vector<R::AllYears::StdDeviation<>> stdDevStsLevel_;
};

} // namespace Antares::Solver::Variable
