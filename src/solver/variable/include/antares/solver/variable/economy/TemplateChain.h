
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
#include <antares/solver/variable/commons/spatial-aggregate.h>

namespace Antares::Solver::Variable::Economy
{
// Variadic template to recursively apply wrappers ending with Tail
// Usage: ApplyChain<Tail, Wrapper1, Wrapper2, ...>::type yields Wrapper1<Wrapper2<...<Tail>...>>
template<typename Tail, template<typename> class... Wrappers>
struct ApplyChain;

template<typename Tail>
struct ApplyChain<Tail>
{
    using type = Tail;
};

template<typename Tail, template<typename> class Head, template<typename> class... Rest>
struct ApplyChain<Tail, Head, Rest...>
{
    using type = Head<typename ApplyChain<Tail, Rest...>::type>;
};

template<template<typename> class... Wrappers>
struct ApplyChainSpatialAgregate;

template<template<typename> class Last>
struct ApplyChainSpatialAgregate<Last>
{
    using type = Common::SpatialAggregate<Last>;
};

template<template<typename> class Head, template<typename> class... Rest>
struct ApplyChainSpatialAgregate<Head, Rest...>
{
    using type = Common::SpatialAggregate<Head, typename ApplyChainSpatialAgregate<Rest...>::type>;
};
} // namespace Antares::Solver::Variable::Economy
