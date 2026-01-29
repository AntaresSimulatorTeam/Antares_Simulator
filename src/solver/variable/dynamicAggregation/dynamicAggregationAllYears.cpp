// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

namespace Antares::Solver::Variable
{

DynamicAggregationAllYears::DynamicAggregationAllYears(Data::Study& study):
    study_(study)
{
    for (const auto& set: study_.setsOfAreas)
    {
        setsData_.try_emplace(set.first, *set.second, study_);
    }
}

void DynamicAggregationAllYears::merge(const DynamicAggregationSingleYear& toMerge, unsigned year)
{
    for (auto& [setName, setData]: setsData_)
    {
        const auto& toMergeSetDataIt = toMerge.setsData_.find(setName);
        if (toMergeSetDataIt != toMerge.setsData_.end())
        {
            const SetDataSingleYear& toMergeSetData = toMergeSetDataIt->second;
            setData.merge(toMergeSetData, study_, year);
        }
    }
}

void DynamicAggregationAllYears::appendToSurveyForSet(const std::string& setName,
                                                      SurveyResults& survey,
                                                      Category::Precision precision) const
{
    auto it = setsData_.find(setName);
    if (it != setsData_.end())
    {
        it->second.appendToSurvey(survey, precision);
    }
}

unsigned int DynamicAggregationAllYears::computeDynamicAggregationMaxColumns() const
{
    unsigned int maxCols = 0;

    for (const auto& [_, setData]: setsData_)
    {
        maxCols += setData.numberOfVariables();
    }
    return maxCols /* *4 */; // TODO: 4 for exp, std, min, max ?
}

} // namespace Antares::Solver::Variable
