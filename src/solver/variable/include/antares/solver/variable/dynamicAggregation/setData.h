// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <set>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/storage/average.h"
#include "antares/solver/variable/storage/minmax.h"
#include "antares/solver/variable/storage/stdDeviation.h"

namespace Antares::Solver::Variable
{

/// Contains common data: group names and areas sets
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

/// Used at the end for each week by adding problem results to Results_ vectors
class SetDataSingleYear: public SetDataBase
{
public:
    friend class SetDataAllYears;

    explicit SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set);

    void addResultsToSet(const PROBLEME_HEBDO& pb);

    void reset();

    void appendToSurvey(SurveyResults& survey,
                        Category::Precision precision,
                        Data::Study& study) const;

    // Getter functions for testing
    const std::vector<std::vector<HighPrecision>>& getThermalResults() const
    {
        return thermalResults_;
    }

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

/// Used at the end of each with merge()
class SetDataAllYears: public SetDataBase
{
public:
    explicit SetDataAllYears(const std::set<Data::Area*, Data::CompareAreaName>& set,
                             Data::Study& study);
    void merge(const SetDataSingleYear& toMerge, Data::Study& study, unsigned year);
    void appendToSurvey(SurveyResults& survey, Category::Precision precision) const;

private:
    void processGroups(const std::vector<R::AllYears::Average>& average,
                       const std::vector<R::AllYears::StdDeviation>& stdDev,
                       const std::vector<R::AllYears::Min>& min,
                       const std::vector<R::AllYears::Max>& max,
                       const std::set<std::string>& groupNames,
                       const Category::Precision& precision,
                       const std::string& suffix,
                       SurveyResults& survey) const;

    std::vector<R::AllYears::Min> minThermal_;
    std::vector<R::AllYears::Max> maxThermal_;

    std::vector<R::AllYears::Min> minRenewable_;
    std::vector<R::AllYears::Max> maxRenewable_;

    std::vector<R::AllYears::Min> minStsInjection_;
    std::vector<R::AllYears::Max> maxStsInjection_;

    std::vector<R::AllYears::Min> minStsWithdrawal_;
    std::vector<R::AllYears::Max> maxStsWithdrawal_;

    std::vector<R::AllYears::Min> minStsLevel_;
    std::vector<R::AllYears::Max> maxStsLevel_;

    std::vector<R::AllYears::Average> averageThermal_;
    std::vector<R::AllYears::Average> averageRenewable_;
    std::vector<R::AllYears::Average> averageStsInjection_;
    std::vector<R::AllYears::Average> averageStsWithdrawal_;
    std::vector<R::AllYears::Average> averageStsLevel_;

    std::vector<R::AllYears::StdDeviation> stdDevThermal_;
    std::vector<R::AllYears::StdDeviation> stdDevRenewable_;
    std::vector<R::AllYears::StdDeviation> stdDevStsInjection_;
    std::vector<R::AllYears::StdDeviation> stdDevStsWithdrawal_;
    std::vector<R::AllYears::StdDeviation> stdDevStsLevel_;
};

} // namespace Antares::Solver::Variable
