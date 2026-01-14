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

#include <antares/study/study.h>
#include "antares/solver/variable/storage/averagedata.h"
#include "antares/study/area/area.h"

namespace Antares::Solver::Variable
{

class SetData
{
public:
    SetData(const std::set<Data::Area*, Data::CompareAreaName>& set);

    // Merge values into results_ using AverageData::merge
    void mergeValues(const std::string& groupName, const std::vector<double>& values);

private:
    std::vector<R::AllYears::AverageData> results_;
    std::vector<std::string> groupNames_;
    std::map<std::string, unsigned int> groupToNumbers_;

    const std::set<Data::Area*, Data::CompareAreaName>& set_;
};

class DynamicAggregation
{
public:
    // Add methods as needed to manage SetData
    void initializeSetsData(const Data::Study& study);

private:
    std::vector<SetData> setsData_;
};

} // namespace Antares::Solver::Variable
