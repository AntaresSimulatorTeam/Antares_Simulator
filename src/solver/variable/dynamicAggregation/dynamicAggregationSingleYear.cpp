// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/dynamicAggregation/dynamicAggregation.h"

namespace Antares::Solver::Variable
{

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

void DynamicAggregationSingleYear::reset()
{
    for (auto& [_, setData]: setsData_)
    {
        setData.reset();
    }
}

void DynamicAggregationSingleYear::appendToSurveyForSet(const std::string& setName,
                                                        SurveyResults& survey,
                                                        Category::Precision precision) const
{
    auto it = setsData_.find(setName);
    if (it != setsData_.end())
    {
        it->second.appendToSurvey(survey, precision, study_);
    }
}

} // namespace Antares::Solver::Variable
