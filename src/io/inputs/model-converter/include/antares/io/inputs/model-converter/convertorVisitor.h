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
#include <algorithm>
#include <antares/expressions/NodeRegistry.h>
#include "antares/io/inputs/yml-model/Library.h"
#include "antares/study/system-model/optimConfig.h"

namespace Antares::IO::Inputs::YmlOptimConfig
{
struct Variable;
}

namespace Antares::IO::Inputs::ModelConverter
{
class NoPortWithThisId final: public std::runtime_error
{
public:
    explicit NoPortWithThisId(const std::string& name);
};

Modeler::Config::Location convertLocation(const std::string& locationStr);

template<class T>
auto SearchEntity(const std::string id, std::vector<std::pair<T*, bool>>& values)
{
    return std::ranges::find_if(values,
                                [&](const auto& optimConfigEntity)
                                { return optimConfigEntity.first->id == id; });
}

template<class T>
auto SearchEntity(const std::string id, const std::vector<std::pair<T*, bool>>& values)
{
    return std::ranges::find_if(values,
                                [&](const auto& optimConfigEntity)
                                { return optimConfigEntity.first->id == id; });
}

template<class T>
Modeler::Config::Location updateLocation(std::string id, std::vector<std::pair<T*, bool>>& values)
{
    if (auto it = SearchEntity(id, values); it != values.end())
    {
        it->second = true;
        return convertLocation(it->first->location);
    }

    return Modeler::Config::Location::SUBPROBLEMS;
}

Expressions::NodeRegistry convertExpressionToNode(
  const std::string& exprStr,
  const YmlModel::Model& model);
} // namespace Antares::IO::Inputs::ModelConverter
