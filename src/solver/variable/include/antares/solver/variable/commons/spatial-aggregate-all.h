// filepath: commons/spatial-aggregate-all.h
#pragma once

#include "antares/solver/variable/commons/spatial-aggregate.h"
#include "antares/solver/variable/variable.h" // for Container::EndOfList

namespace Antares::Solver::Variable::Common
{

// Variadic composition of SpatialAggregate: SpatialAggregateAll<VarT1, VarT2>::type =
// SpatialAggregate<VarT1, SpatialAggregate<VarT2, Container::EndOfList>>
template<template<class> class Head, template<class> class... Tail>
struct SpatialAggregateAll
{
    using type = SpatialAggregate<Head, typename SpatialAggregateAll<Tail...>::type>;
};

// Base case: single variable
template<template<class> class Last>
struct SpatialAggregateAll<Last>
{
    using type = SpatialAggregate<Last, Container::EndOfList>;
};

} // namespace Antares::Solver::Variable::Common
