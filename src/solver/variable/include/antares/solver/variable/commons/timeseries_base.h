// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
 * TimeSeriesValuesBase<D,V>    ← Base behavior implementation
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
 * class MyTimeSeries : public TimeSeriesValuesBase<MyTimeSeries, VCardMyTimeSeries> {
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
    using ResultsType = StandardResults<>;

    /// Data aggregation level (area-based)
    static constexpr uint8_t categoryDataLevel = Category::DataLevel::area;
    /// File output level configuration
    static constexpr uint8_t categoryFileLevel = ResultsType::categoryFile
                                                 & (Category::FileLevel::id
                                                    | Category::FileLevel::va);
    /// Precision settings for output
    static constexpr uint8_t precision = Category::all;
    /// GUI indentation level
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

/**
 * @brief Base implementation for time series variables using CRTP pattern
 * * This class provides the common functionality for all time series variables.
 * It uses the Curiously Recurring Template Pattern (CRTP) to achieve static
 * polymorphism without virtual function overhead.
 *
 * @tparam Derived The derived class (CRTP)
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
template<typename Derived, typename VCardType>
class TimeSeriesValuesBase: public Variable::IVariable<Derived, VCardType>
{
public:
    using AncestorType = Variable::IVariable<Derived, VCardType>;
    using ResultsType = typename VCardType::ResultsType;
    using VariableAccessorType = VariableAccessor<ResultsType, VCardType::columnCount>;

    static constexpr std::size_t count = 1;

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        static constexpr int count = ((VCardType::categoryDataLevel & CDataLevel
                                       && VCardType::categoryFileLevel & CFile)
                                        ? VCardType::columnCount * ResultsType::count
                                        : 0);
    };

    void initializeFromStudy(Data::Study& study)
    {
        nbYearsParallel = study.maxNbYearsInParallel;

        InitializeResultsFromStudy(AncestorType::pResults, study);

        yearlyValues.resize(nbYearsParallel);
        std::for_each(yearlyValues.begin(),
                      yearlyValues.end(),
                      [&study](auto& value) { value.initializeFromStudy(study); });

        static_cast<Derived*>(this)->initializeDerivedFromStudy(study);
    }

    template<class R>
    static void InitializeResultsFromStudy(R& results, Data::Study& study)
    {
        VariableAccessorType::InitializeAndReset(results, study);
    }

    void initializeFromArea(Data::Study* study, Data::Area* area) noexcept
    {
        areaPtr = area;
    }

    void simulationBegin()
    {
        std::for_each(yearlyValues.begin(), yearlyValues.end(), [](auto& value) { value.reset(); });
    }

    void yearBegin(unsigned int year, unsigned int space)
    {
        static_cast<Derived*>(this)->yearBeginImpl(year, space);
    }

    void yearEnd(unsigned int year, unsigned int space)
    {
        yearlyValues[space].computeStatisticsForTheCurrentYear();
    }

    void computeSummary(unsigned int year, unsigned int space)
    {
        AncestorType::pResults.merge(year, yearlyValues[space]);
    }

    void hourForEachArea(State& state, unsigned int space)
    {
        static_cast<Derived*>(this)->hourForEachAreaImpl(state, space);
    }

    Antares::Memory::Stored<double>::ConstReturnType retrieveRawHourlyValuesForCurrentYear(
      unsigned int,
      unsigned int space) const noexcept
    {
        return yearlyValues[space].hour;
    }

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
    Data::Area* areaPtr{nullptr};
    typename VCardType::IntermediateValuesType yearlyValues;

    unsigned int nbYearsParallel{0};
};

} // namespace Antares::Solver::Variable::Economy
