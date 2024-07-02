/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

//
// Created by marechaljas on 27/06/23.
//

#pragma once

#include <vector>

namespace Antares
{
namespace Data
{

struct BindingConstraintStructures
{
    mutable std::vector<long> linkIndex;
    mutable std::vector<double> linkWeight;
    mutable std::vector<double> clusterWeight;
    mutable std::vector<int> linkOffset;
    mutable std::vector<int> clusterOffset;
    mutable std::vector<long> clusterIndex;
    mutable std::vector<long> clustersAreaIndex;
};

} // namespace Data
} // namespace Antares
