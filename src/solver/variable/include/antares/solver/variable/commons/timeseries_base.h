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

/**
 * @file timeseries_base.h
 * @brief Base classes and traits for time series variables in Antares Simulator
 * * This file provides a modern C++20 foundation for implementing time series variables
 * in the Antares Simulator solver. It eliminates code duplication by providing
 * common functionality through templates and traits.
 *
 * ## Architecture Overview
 *
 * ```
 * TimeSeriesTraits<T>          ← Common properties and types
 *        ↑
 * VCardTimeSeriesBase<T>       ← VCard implementation
 *        ↑
 * TimeSeriesValuesBase<D,N,V>  ← Base behavior implementation
 *        ↑
 * Derived Classes              ← Specific implementations (Load, Solar, etc.)
 * ```
 *
 * ## Usage Example
 *
 * ```cpp
 * // 1. Define traits for your time series type
 * struct MyTimeSeriesTraits {
 *     static constexpr std::string_view kCaption = "MY_TYPE";
 *     static constexpr std::string_view kDescription = "My custom time series";
 * };
 *
 * // 2. Create VCard using the base template
 * using VCardMyTimeSeries = VCardTimeSeriesBase<MyTimeSeriesTraits>;
 *
 * // 3. Implement your time series class
 * template<class NextT = Container::EndOfList>
 * class MyTimeSeries : public TimeSeriesValuesBase<MyTimeSeries<NextT>, NextT, VCardMyTimeSeries> {
 * public:
 *     void initializeDerivedFromStudy(Data::Study& study) { ... }
 *     void yearBeginImpl(unsigned int year, unsigned int space) { ... }
 *     void hourForEachAreaImpl(State& state, unsigned int space) { ... }
 * };
 * ```
 *
 * @see generation.h for concrete examples
 * @see load.h, hydro.h for other implementations
 */

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <antares/study/area/area.h>
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Economy
{

/**
 * @brief Base traits template providing common properties for all time series variables
 * * This template provides default values for VCard properties that are common across
 * all time series variables. Derived traits only need to specify what makes them unique.
 * * @tparam Derived The derived traits type (CRTP pattern)
 * * ## Common Properties Provided:
 * - Unit: "MWh" for all time series
 * - Data level: Area-level aggregation
 * - File output configuration
 * - Spatial aggregation settings
 * - Intermediate values configuration
 * * ## Type Definitions:
 * - `ResultsType`: Configured for average, std deviation, min, max across all years
 * - `IntermediateValuesType`: Vector of intermediate values for parallel processing
 */
template<typename Derived>
struct TimeSeriesTraits
{
    /// Unit for all time series variables
    static constexpr std::string_view kUnit = "MWh";

    /// Results configuration: Average, StdDev, Min, Max across all years
    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    /// Data aggregation level (area-based)
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    /// File output level configuration
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    /// Precision settings for output
    static constexpr uint8_t precision = Category::all;
    /// GUI indentation level
    static constexpr uint8_t nodeDepthForGUI = 0;
    /// Decimal precision for display
    static constexpr uint8_t decimal = 0;
    /// Number of columns in output (typically 1 for time series)
    static constexpr int columnCount = 1;
    /// Spatial aggregation method (sum across areas)
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;
    /// Spatial aggregation mode (each year separately)
    static constexpr uint8_t spatialAggregateMode = Category::spatialAggregateEachYear;
    /// Post-processing for spatial aggregation
    static constexpr uint8_t spatialAggregatePostProcessing = 0;
    /// Whether intermediate values are computed
    static constexpr uint8_t hasIntermediateValues = 1;
    /// Whether variable can be non-applicable
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    /// Base type for intermediate values storage
    using IntermediateValuesBaseType = IntermediateValues;
    /// Container for intermediate values (one per parallel space)
    using IntermediateValuesType = std::vector<IntermediateValues>;
    /// Pointer type for spatial aggregation
    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;
};

/**
 * @brief VCard template providing variable metadata and configuration
 * * This template creates a VCard (Variable Card) that describes a time series variable.
 * It combines the common traits with specific traits provided by the TraitsType parameter.
 * * @tparam TraitsType Specific traits defining caption, description, and any custom properties
 */
template<typename TraitsType>
struct VCardTimeSeriesBase: public TimeSeriesTraits<TraitsType>
{
    /// Base traits type
    using BaseType = TimeSeriesTraits<TraitsType>;
    /// Self-reference for spatial aggregation
    using VCardForSpatialAggregate = VCardTimeSeriesBase<TraitsType>;

    /// Modern C++20 API: compile-time string views
    inline static constexpr std::string_view kCaption = TraitsType::kCaption;
    inline static constexpr std::string_view kUnit = BaseType::kUnit;
    inline static constexpr std::string_view kDescription = TraitsType::kDescription;

    /// @name Legacy API (backward compatibility)
    /// @{
    /**
     * @brief Get variable caption as std::string
     * @return Variable caption (e.g., "SOLAR", "WIND", "LOAD")
     */
    static std::string Caption()
    {
        return std::string(kCaption);
    }

    /**
     * @brief Get variable unit as std::string
     * @return Variable unit ("MWh")
     */
    static std::string Unit()
    {
        return std::string(kUnit);
    }

    /**
     * @brief Get variable description as std::string
     * @return Detailed description of the variable
     */
    static std::string Description()
    {
        return std::string(kDescription);
    }

    /// @}

    /// @name Type Definitions
    /// @{
    using ResultsType = typename BaseType::ResultsType;
    using IntermediateValuesBaseType = typename BaseType::IntermediateValuesBaseType;
    using IntermediateValuesType = typename BaseType::IntermediateValuesType;
    using IntermediateValuesTypeForSpatialAg = typename BaseType::
      IntermediateValuesTypeForSpatialAg;
    /// @}
};

/**
 * @brief Internal helpers for template metaprogramming
 */
namespace detail
{
/**
 * @brief Compile-time calculation of statistics count
 * * This helper computes the number of statistics that will be generated based on
 * the data level and file level filters.
 * * @tparam VCard The variable card type
 * @tparam Next The next variable in the chain
 * @tparam CDataLevel Data level filter
 * @tparam CFile File level filter
 */
template<class VCard, class Next, int CDataLevel, int CFile>
inline constexpr int StatisticsCount = ((VCard::categoryDataLevel & CDataLevel
                                         && VCard::categoryFileLevel & CFile)
                                          ? (Next::template Statistics<CDataLevel, CFile>::count
                                             + VCard::columnCount * VCard::ResultsType::count)
                                          : Next::template Statistics<CDataLevel, CFile>::count);
} // namespace detail

/**
 * @brief Base implementation for time series variables using CRTP pattern
 * * This class provides the common functionality for all time series variables.
 * It uses the Curiously Recurring Template Pattern (CRTP) to achieve static
 * polymorphism without virtual function overhead.
 *
 * @tparam Derived The derived class (CRTP)
 * @tparam NextT The next variable in the chain (for variable composition)
 * @tparam VCardType The VCard describing this variable
 *
 * ## Derived Class Requirements:
 *
 * Derived classes must implement these methods:
 *
 * ```cpp
 * void initializeDerivedFromStudy(Data::Study& study);
 * void yearBeginImpl(unsigned int year, unsigned int space);
 * void hourForEachAreaImpl(State& state, unsigned int space);
 * ```
 *
 * ## Lifecycle Methods:
 *
 * The simulation follows this lifecycle:
 * 1. `initializeFromStudy()` - Setup from study configuration
 * 2. `initializeFromArea()` - Associate with specific area
 * 3. `simulationBegin()` - Reset state for new simulation
 * 4. For each year:
 *    - `yearBegin()` - Setup for year
 *    - For each hour: `hourForEachArea()` - Process hourly data
 *    - `yearEnd()` - Compute year statistics
 *    - `computeSummary()` - Merge with global results
 * 5. `simulationEnd()` - Cleanup
 *
 * ## Memory Management:
 *
 * The class manages memory automatically using RAII principles:
 * - `yearlyValues`: Automatically sized vector of intermediate values
 * - `areaPtr`: Non-owning pointer to area (managed externally)
 * - `nbYearsParallel`: Cached for performance
 */
template<typename Derived, typename NextT, typename VCardType>
class TimeSeriesValuesBase: public Variable::IVariable<Derived, NextT, VCardType>
{
public:
    /// @name Type Definitions
    /// @{
    using NextType = NextT;
    using AncestorType = Variable::IVariable<Derived, NextT, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;
    /// @}

    /// Total count of variables in the chain
    static constexpr int count = 1 + NextT::count;

    /**
     * @brief Compile-time statistics calculation
     *     * This nested template computes how many statistics will be generated
     * for different data and file level combinations.
     */
    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = detail::
          StatisticsCount<VCardType, NextType, CDataLevel, CFile>;
    };

    /**
     * @brief Initialize the variable from study configuration
     *     * This method sets up the variable based on the study parameters,
     * allocates necessary memory, and initializes the result structures.
     *     * @param study The study configuration containing simulation parameters
     *     * ## Initialization Steps:
     * 1. Cache the number of parallel years for performance
     * 2. Initialize result storage structures
     * 3. Resize and initialize yearly value containers
     * 4. Call derived class initialization
     * 5. Initialize the next variable in chain
     */
    void initializeFromStudy(Data::Study& study)
    {
        // Cache for performance - used frequently during simulation
        nbYearsParallel = study.maxNbYearsInParallel;

        // Initialize the results storage with proper structure
        InitializeResultsFromStudy(AncestorType::pResults, study);

        // Allocate intermediate values for each parallel space
        yearlyValues.resize(nbYearsParallel);
        std::for_each(yearlyValues.begin(),
                      yearlyValues.end(),
                      [&study](auto& value) { value.initializeFromStudy(study); });

        // Allow derived class to perform specific initialization
        static_cast<Derived*>(this)->initializeDerivedFromStudy(study);
        // Initialize next variable in chain
        NextType::initializeFromStudy(study);
    }

    /**
     * @brief Static helper to initialize results from study
     *     * @tparam R Results type
     * @param results Results structure to initialize
     * @param study Study configuration
     */
    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    /**
     * @brief Associate this variable with a specific area
     *     * @param study Study configuration (unused but kept for interface compatibility)
     * @param area The area this variable is associated with
     */
    void initializeFromArea(Data::Study* study, Data::Area* area) noexcept
    {
        areaPtr = area;
        NextType::initializeFromArea(study, area);
    }

    /**
     * @brief Initialize from link (for link-based variables)
     *     * @param study Study configuration
     * @param link The link this variable is associated with
     */
    void initializeFromLink(Data::Study* study, Data::AreaLink* link)
    {
        NextType::initializeFromAreaLink(study, link);
    }

    /**
     * @brief Reset state at the beginning of simulation
     *     * Clears all intermediate values to prepare for a new simulation run.
     */
    void simulationBegin()
    {
        std::for_each(yearlyValues.begin(), yearlyValues.end(), [](auto& value) { value.reset(); });
        NextType::simulationBegin();
    }

    /**
     * @brief Cleanup at the end of simulation
     */
    void simulationEnd()
    {
        NextType::simulationEnd();
    }

    /**
     * @brief Setup at the beginning of each year
     *     * @param year The year index (0-based)
     * @param space The parallel space index
     */
    void yearBegin(unsigned int year, unsigned int space)
    {
        static_cast<Derived*>(this)->yearBeginImpl(year, space);
        NextType::yearBegin(year, space);
    }

    /**
     * @brief Post-processing after year end build
     *     * @param state Current simulation state
     * @param year The year index
     * @param space The parallel space index
     */
    void yearEndBuild(State& state, unsigned int year, unsigned int space)
    {
        NextType::yearEndBuild(state, year, space);
    }

    /**
     * @brief Finalize year and compute statistics
     *     * @param year The year index
     * @param space The parallel space index
     */
    void yearEnd(unsigned int year, unsigned int space)
    {
        yearlyValues[space].computeStatisticsForTheCurrentYear();
        NextType::yearEnd(year, space);
    }

    /**
     * @brief Merge year results into global summary
     *     * @param year The year index
     * @param space The parallel space index
     */
    void computeSummary(unsigned int year, unsigned int space)
    {
        AncestorType::pResults.merge(year, yearlyValues[space]);
        NextType::computeSummary(year, space);
    }

    /**
     * @brief Setup at the beginning of each hour
     *     * @param hourInTheYear Hour index within the year (0-8759)
     */
    void hourBegin(unsigned int hourInTheYear)
    {
        NextType::hourBegin(hourInTheYear);
    }

    /**
     * @brief Process data for each area during an hour
     *     * This is where the actual time series data processing happens.
     *     * @param state Current simulation state
     * @param space The parallel space index
     */
    void hourForEachArea(State& state, unsigned int space)
    {
        static_cast<Derived*>(this)->hourForEachAreaImpl(state, space);
        NextType::hourForEachArea(state, space);
    }

    /**
     * @brief Cleanup at the end of each hour
     *     * @param state Current simulation state
     * @param hourInTheYear Hour index within the year
     */
    void hourEnd(State& state, unsigned int hourInTheYear)
    {
        NextType::hourEnd(state, hourInTheYear);
    }

    /**
     * @brief Retrieve raw hourly values for the current year
     *     * @param unused Unused parameter (kept for interface compatibility)
     * @param space The parallel space index
     * @return Const reference to hourly values array
     */
    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int space) const noexcept
    {
        return yearlyValues[space].hour;
    }

    /**
     * @brief Build annual survey report for output
     *     * @param results Survey results structure to populate
     * @param fileLevel File output level filter
     * @param precision Precision requirements
     * @param space The parallel space index
     */
    void localBuildAnnualSurveyReport(SurveyResults& results,
                                      int fileLevel,
                                      int precision,
                                      unsigned int space) const
    {
        results.isCurrentVarNA = AncestorType::isNonApplicable;
        if (AncestorType::isPrinted[0])
        {
            results.variableCaption = VCardType::kCaption.data();
            results.variableUnit = std::string(VCardType::kUnit);
            yearlyValues[space].template buildAnnualSurveyReport<VCardType>(results,
                                                                            fileLevel,
                                                                            precision);
        }
    }

protected:
    /// @name Protected Members
    /// @{
    /// Non-owning pointer to the associated area
    Data::Area* areaPtr{nullptr};
    /// Intermediate values for each parallel space
    typename VCardType::IntermediateValuesType yearlyValues;

    /// Number of parallel years (cached for performance)
    unsigned int nbYearsParallel{0};

    /// @}
};

} // namespace Antares::Solver::Variable::Economy
