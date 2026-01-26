// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
