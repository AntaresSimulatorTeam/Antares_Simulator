// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <string>
#include <vector>

#include "antares/solver/variable/storage/results.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

// ============================================================================
// Traits classes for Reserve Participation VCard variants
// ============================================================================

// Common result type definitions
namespace ResultTypes
{
using Standard = StandardResults<R::AllYears::Average>;

using AverageOnly = Results<std::tuple<R::AllYears::Average>>;
} // namespace ResultTypes

// Category level definitions
namespace CategoryLevels
{
constexpr int Standard = Category::FileLevel::de;
constexpr int GroupLevel = Category::FileLevel::id | Category::FileLevel::va;
} // namespace CategoryLevels

// Separate detail structs
namespace Detail
{
struct DispatchableOff
{
    static constexpr std::string_view caption = "OFF UNITS CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view unit = "Reserve Participation Power - MWh";
    static constexpr std::string_view description = "Reserve Participation from off units in "
                                                    "cluster to a reserve";
};

struct DispatchableOn
{
    static constexpr std::string_view caption = "RUNNING UNITS CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view unit = "Reserve Participation Power - MWh";
    static constexpr std::string_view description = "Reserve Participation from running units in "
                                                    "cluster to a reserve";
};

struct Hydro
{
    static constexpr std::string_view caption = "HYDRO RESERVE PARTICIPATION";
    static constexpr std::string_view unit = "Reserve Participation Power - MWh";
    static constexpr std::string_view description = "Reserve Participation from hydro to a reserve";
};

struct STStorage
{
    static constexpr std::string_view caption = "CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view unit = "Reserve Participation Power - MWh";
    static constexpr std::string_view description = "Reserve Participation from a cluster to a "
                                                    "reserve";
};

struct STStorageGroup
{
    static constexpr std::string_view caption = "SHORT TERM STORAGE GROUP PARTICIPATION TO RESERVE";
    static constexpr std::string_view unit = "MWh";
    static constexpr std::string_view description = "Reserve Participation from a group to a "
                                                    "reserve";
};

struct ThermalGroup
{
    static constexpr std::string_view caption = "THERMAL GROUP PARTICIPATION TO RESERVE";
    static constexpr std::string_view unit = "MWh";
    static constexpr std::string_view description = "Reserve Participation from a group to a "
                                                    "reserve";
};

struct MarginalCost
{
    static constexpr std::string_view caption = "RESERVE PARTICIPATION MARGINAL COST";
    static constexpr std::string_view unit = "Euro";
    static constexpr std::string_view description = "Reserve participation marginal cost";
};

struct UnsuppliedSpilled
{
    static constexpr std::string_view caption = "RESERVE UNSUPPLIED OR SPILLED ENERGY";
    static constexpr std::string_view unit = "MWh";
    static constexpr std::string_view description = "Reserve unsupplied or spilled Power";
};
} // namespace Detail

// Traits template - now Details is complete when accessed
template<typename Details,
         int CategoryFileLevel = CategoryLevels::Standard,
         int Decimal = 2,
         typename Results = ResultTypes::Standard>
struct ReserveParticipationTraits
{
    static constexpr std::string_view kCaption = Details::caption;
    static constexpr std::string_view kUnit = Details::unit;
    static constexpr std::string_view kDescription = Details::description;
    static constexpr int kCategoryFileLevel = CategoryFileLevel;
    static constexpr int kDecimal = Decimal;
    using ResultsType = Results;
};

// Final trait types - clean and explicit
using DispatchableOffTraits = ReserveParticipationTraits<Detail::DispatchableOff>;
using DispatchableOnTraits = ReserveParticipationTraits<Detail::DispatchableOn>;
using HydroTraits = ReserveParticipationTraits<Detail::Hydro>;
using STStorageTraits = ReserveParticipationTraits<Detail::STStorage>;

using STStorageGroupTraits = ReserveParticipationTraits<Detail::STStorageGroup,
                                                        CategoryLevels::GroupLevel>;
using ThermalGroupTraits = ReserveParticipationTraits<Detail::ThermalGroup,
                                                      CategoryLevels::GroupLevel>;
using MarginalCostTraits = ReserveParticipationTraits<Detail::MarginalCost,
                                                      CategoryLevels::GroupLevel>;
using UnsuppliedSpilledTraits = ReserveParticipationTraits<Detail::UnsuppliedSpilled,
                                                           CategoryLevels::GroupLevel,
                                                           0,
                                                           ResultTypes::AverageOnly>;

// ============================================================================
// Generic VCard template for Reserve Participation
// ============================================================================

/**
 * @brief Generic VCard template for reserve participation types
 *
 * Uses traits-based customization to avoid code duplication across
 * different reserve participation VCard variants.
 */
template<typename TraitsType>
struct VCardReserveParticipationBase
{
    //! Caption
    static std::string Caption()
    {
        return std::string(TraitsType::kCaption);
    }

    //! Unit
    static std::string Unit()
    {
        return std::string(TraitsType::kUnit);
    }

    //! The short description of the variable
    static std::string Description()
    {
        return std::string(TraitsType::kDescription);
    }

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardReserveParticipationBase VCardForSpatialAggregate;

    //! The expected results type from traits
    using ResultsType = typename TraitsType::ResultsType;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::DataLevel::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & TraitsType::kCategoryFileLevel,
        //! Precision (views)
        precision = Category::all,
        //! Decimal precision
        decimal = TraitsType::kDecimal,
        //! Number of columns used by the variable
        columnCount = Category::dynamicColumns,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;

}; // class VCardReserveParticipationBase

} // namespace Antares::Solver::Variable::Economy::Reserves
