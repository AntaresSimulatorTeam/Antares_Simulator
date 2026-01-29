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

#include <map>
#include <string>

#include "antares/solver/variable/dynamicAggregation/setData.h"

namespace Antares::Solver::Variable
{

class DynamicAggregationSingleYear
{
public:
    friend class DynamicAggregationAllYears;

    explicit DynamicAggregationSingleYear(Data::Study& study);

    void addResultsToSets(const PROBLEME_HEBDO& pb);

    void appendToSurveyForSet(const std::string& setName,
                              SurveyResults& survey,
                              Category::Precision precision) const;

private:
    Data::Study& study_;
    std::map<std::string, SetDataSingleYear> setsData_;
};

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
