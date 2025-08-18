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

#include "categories.h"

namespace Antares::Solver::Variable::detail {

template<class VCardT>
struct variable_category_traits {
    static constexpr bool is_single  = (VCardT::columnCount == Category::singleColumn);
    static constexpr bool is_dynamic = (VCardT::columnCount == Category::dynamicColumns);
    static constexpr bool is_multiple = (VCardT::columnCount > 1) && !is_dynamic; // colonnes statiques multiples
    static constexpr int effective_column_count = is_multiple ? VCardT::columnCount : 1; // dynamique, single, noColumn -> 1
};

} // namespace Antares::Solver::Variable::detail

