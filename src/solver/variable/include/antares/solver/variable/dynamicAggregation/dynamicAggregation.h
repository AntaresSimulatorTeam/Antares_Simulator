// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>

#include "antares/solver/variable/dynamicAggregation/setData.h"

namespace Antares::Solver::Variable
{

/// Used at the end of each week by adding problem results to SetDataSingleYear
/// SetDataSingleYear contains vectors<long double> for each group for each type of results
/// results: thermal, renewable, sts injection, sts withdrawal, sts level
/// Exporting is only 1 column per group, we handle calendar with IntermediateValues
class DynamicAggregationSingleYear
{
public:
    friend class DynamicAggregationAllYears;

    explicit DynamicAggregationSingleYear(Data::Study& study);

    void addResultsToSets(const PROBLEME_HEBDO& pb);

    void reset();

    void appendToSurveyForSet(const std::string& setName,
                              SurveyResults& survey,
                              Category::Precision precision) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataSingleYear> setsData_;
};

/// Used at the end of each year to aggregate DynamicAggregationSingleYear results
/// Contains Average, StdDev, Min, Max for each group for each type of results
/// results: thermal, renewable, sts injection, sts withdrawal, sts level
/// Exporting is only 4 column per group
class DynamicAggregationAllYears
{
public:
    explicit DynamicAggregationAllYears(Data::Study& study);

    void merge(const DynamicAggregationSingleYear& toMerge, unsigned year);

    void appendToSurveyForSet(const std::string& setName,
                              SurveyResults& survey,
                              Category::Precision precision) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataAllYears> setsData_;
};

unsigned int computeDynamicAggregationMaxColumns(const Data::Study& study);

} // namespace Antares::Solver::Variable
