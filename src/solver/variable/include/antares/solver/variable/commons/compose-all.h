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

#include "antares/solver/variable/variable.h" // for Container::EndOfList

namespace Antares::Solver::Variable::Common
{

// Variadic recursive composition of class templates: ComposeAll<Head, Tail...>::type =
// Head<Tail<...<Container::EndOfList>>>
template<template<class> class Head, template<class> class... Tail>
struct ComposeAll
{
    using type = Head<typename ComposeAll<Tail...>::type>;
};

// Base case: single template
template<template<class> class Last>
struct ComposeAll<Last>
{
    using type = Last<Container::EndOfList>;
};

} // namespace Antares::Solver::Variable::Common
