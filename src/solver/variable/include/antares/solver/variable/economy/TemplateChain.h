
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
#include <type_traits> // detection idiom for ChainLinks base case

#include <antares/solver/variable/container.h> // for Container::EndOfList

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
// Common::SpatialAggregate<W1, Common::SpatialAggregate<W2, ...>>
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

template<template<typename> class... Wrappers>
struct ChainLinks;

template<template<typename = Container::EndOfList> class Last>
struct ChainLinks<Last>
{
    using type = Last<>;
};

// Recursive case
template<template<typename> class Head, template<typename> class... Rest>
struct ChainLinks<Head, Rest...>
{
    using type = Head<typename ChainLinks<Rest...>::type>;
};

template<typename C1, typename C2>
struct Fuse;

template<template<typename = void> class... W1, template<typename = void> class... W2>
struct Fuse<ChainLinks<W1...>, ChainLinks<W2...>>
{
    using type = typename ChainLinks<W1..., W2...>::type;
};
} // namespace Antares::Solver::Variable::Economy
