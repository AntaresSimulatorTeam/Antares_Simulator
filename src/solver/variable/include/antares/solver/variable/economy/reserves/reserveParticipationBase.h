/*
** Copyright 2007-2025 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#pragma once

#include <string>
#include <vector>

#include "antares/solver/variable/storage/results.h"

namespace Antares::Solver::Variable::Economy::Reserves
{

// ============================================================================
// Traits classes for Reserve Participation VCard variants
// ============================================================================

/**
 * @brief Base traits for reserve participation VCard
 *
 * Each reserve participation type should define a traits class that specifies
 * Caption, Unit, Description, categoryFileLevel, and decimal precision.
 */
struct DispatchableOffTraits
{
    static constexpr std::string_view kCaption = "OFF UNITS CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view kUnit = "Reserve Participation Power - MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from off units in "
                                                     "cluster to a reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::de;
    static constexpr int kDecimal = 2;
};

struct DispatchableOnTraits
{
    static constexpr std::string_view kCaption = "RUNNING UNITS CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view kUnit = "Reserve Participation Power - MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from running units in "
                                                     "cluster to a reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::de;
    static constexpr int kDecimal = 2;
};

struct HydroTraits
{
    static constexpr std::string_view kCaption = "HYDRO RESERVE PARTICIPATION";
    static constexpr std::string_view kUnit = "Reserve Participation Power - MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from hydro to a "
                                                     "reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::de;
    static constexpr int kDecimal = 2;
};

struct STStorageTraits
{
    static constexpr std::string_view kCaption = "CLUSTER PARTICIPATION TO RESERVE";
    static constexpr std::string_view kUnit = "Reserve Participation Power - MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from a cluster to a "
                                                     "reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::de;
    static constexpr int kDecimal = 2;
};

struct STStorageGroupTraits
{
    static constexpr std::string_view kCaption = "SHORT TERM STORAGE GROUP PARTICIPATION TO "
                                                 "RESERVE";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from a group to a "
                                                     "reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::id | Category::FileLevel::va;
    static constexpr int kDecimal = 2;
};

struct ThermalGroupTraits
{
    static constexpr std::string_view kCaption = "THERMAL GROUP PARTICIPATION TO RESERVE";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Reserve Participation from a group to a "
                                                     "reserve";
    static constexpr int kCategoryFileLevel = Category::FileLevel::id | Category::FileLevel::va;
    static constexpr int kDecimal = 2;
};

struct MarginalCostTraits
{
    static constexpr std::string_view kCaption = "RESERVE PARTICIPATION MARGINAL COST";
    static constexpr std::string_view kUnit = "Euro";
    static constexpr std::string_view kDescription = "Reserve participation marginal cost";
    static constexpr int kCategoryFileLevel = Category::FileLevel::id | Category::FileLevel::va;
    static constexpr int kDecimal = 2;
};

struct UnsuppliedSpilledTraits
{
    static constexpr std::string_view kCaption = "RESERVE UNSUPPLIED OR SPILLED ENERGY";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Reserve unsupplied or spilled Power";
    static constexpr int kCategoryFileLevel = Category::FileLevel::id | Category::FileLevel::va;
    static constexpr int kDecimal = 0;
};

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

    //! The expected results
    typedef Results<R::AllYears::Average<>> ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardReserveParticipationBase VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::DataLevel::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & TraitsType::kCategoryFileLevel,
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
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
