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
 * @file generation.h
 * @brief Generic renewable generation variables for solar and wind power
 * * This file provides a unified implementation for renewable generation time series
 * variables (solar and wind). It demonstrates the practical application of the
 * time series base framework defined in timeseries_base.h.
 *
 *
 * ## Generation-Specific Behavior:
 *
 * The generation variables have special behavior for renewable energy:
 * - Data is copied at year beginning if aggregation is enabled
 * - No hourly processing needed (values pre-aggregated)
 * - Respects study configuration for renewable generation aggregation
 *
 * @see timeseries_base.h for the base framework
 * @see load.h, hydro.h for other time series implementations
 */
#pragma once

#include <algorithm>
#include <string_view>

#include <antares/study/area/area.h>

#include "timeseries_base.h"

namespace Antares::Solver::Variable::Economy
{

/// @name Generation Type Tags
/// @{

/**
 * @brief Tag type for solar generation variables
 * * Used in the legacy tag-based template system for backward compatibility.
 * New code should use SolarTraits directly.
 */
struct SolarTag
{
};

/**
 * @brief Tag type for wind generation variables
 * * Used in the legacy tag-based template system for backward compatibility.
 * New code should use WindTraits directly.
 */
struct WindTag
{
};

/// @}

/// @name Generation Traits
/// @{

/**
 * @brief Traits for solar generation time series
 * * Defines the specific characteristics of solar generation variables:
 * - Caption: "SOLAR" (as displayed in outputs)
 * - Description: Descriptive text for documentation
 * - Area Member: Points to Data::Area::solar for data access
 * * ## Data Access:
 * The areaMember pointer allows type-safe access to the correct area data:
 * ```cpp
 * auto& solarData = (area->*SolarTraits::areaMember);
 * // Equivalent to: auto& solarData = area->solar;
 * ```
 */
struct SolarTraits
{
    /// Display name for solar generation in outputs and GUI
    inline static constexpr std::string_view kCaption = "SOLAR";
    /// Descriptive text (preserving original typo for stability)
    inline static constexpr std::string_view kDescription = "Solar generation, thoughout all MC "
                                                            "years";
    // Pointer-to-member for accessing solar data from Area
    static constexpr auto areaMember = &Data::Area::solar;
};

/**
 * @brief Traits for wind generation time series
 * * Defines the specific characteristics of wind generation variables:
 * - Caption: "WIND" (as displayed in outputs)
 * - Description: Descriptive text for documentation* - Area Member: Points to Data::Area::wind for
 * - Description: Descriptive text for documentation
 * - Area Member: Points to Data::Area::wind for data access
 * * ## Data Access:
 * The areaMember pointer allows type-safe access to the correct area data:
 * ```cpp
 * auto& windData = (area->*WindTraits::areaMember);
 * // Equivalent to: auto& windData = area->wind;
 * ```
 */
struct WindTraits
{
    /// Display name for wind generation in outputs and GUI
    inline static constexpr std::string_view kCaption = "WIND";
    /// Descriptive text (preserving original typo for stability)
    inline static constexpr std::string_view kDescription = "Wind generation, thoughout all MC "
                                                            "years";
    // Pointer-to-member for accessing wind data from Area
    static constexpr auto areaMember = &Data::Area::wind;
};

/// @}

/// @name VCard Type Aliases
/// @{

/**
 * @brief VCard for solar generation variables
 * * Provides metadata and configuration for solar generation time series.
 * Inherits common properties from TimeSeriesTraits and adds solar-specific
 * information from SolarTraits.
 */
using VCardTimeSeriesValuesSolar = VCardTimeSeriesBase<SolarTraits>;

/**
 * @brief VCard for wind generation variables
 * * Provides metadata and configuration for wind generation time series.
 * Inherits common properties from TimeSeriesTraits and adds wind-specific
 * information from WindTraits.
 */
using VCardTimeSeriesValuesWind = VCardTimeSeriesBase<WindTraits>;

/// @}

/// @name Legacy Compatibility Layer
/// @{

/**
 * @brief Primary template for generation traits mapping (undefined)
 * * This template is specialized for each tag type to provide trait mapping.
 * The primary template is intentionally undefined to catch usage with
 * unsupported tag types at compile time.
 * * @tparam Tag The generation tag type (SolarTag or WindTag)
 */
template<class Tag>
struct GenerationTraits; // primary template (undefined)

/**
 * @brief Specialization mapping SolarTag to SolarTraits
 * * Provides backward compatibility by mapping the old tag-based system
 * to the new traits-based system.
 */
template<>
struct GenerationTraits<SolarTag>: public SolarTraits
{
};

/**
 * @brief Specialization mapping WindTag to WindTraits
 * * Provides backward compatibility by mapping the old tag-based system
 * to the new traits-based system.
 */
template<>
struct GenerationTraits<WindTag>: public WindTraits
{
};

/**
 * @brief Legacy VCard template using tag-based traits
 * * Provides backward compatibility for code using the old tag-based approach.
 * New code should use VCardTimeSeriesValuesSolar/Wind directly.
 * * @tparam Tag Generation tag type (SolarTag or WindTag)
 */
template<class Tag>
using VCardTimeSeriesValuesGeneration = VCardTimeSeriesBase<GenerationTraits<Tag>>;

/// @}

/// @name Implementation Classes
/// @{

/**
 * @brief Common implementation for renewable generation time series
 * * This class provides the shared implementation for both solar and wind
 * generation variables. It handles the specific behavior of renewable
 * generation data processing.
 * * @tparam TraitsType The traits defining the specific generation type
 * @tparam NextT The next variable in the processing chain
 * * ## Key Behaviors:
 * * ### Year Begin Processing:
 * - Checks if renewable generation aggregation is enabled
 * - If enabled, copies the entire year's data at once for performance
 * - Uses the areaMember from traits for type-safe data access
 * * ### Hourly Processing:
 * - No specific hourly processing (data already copied in yearBegin)
 * - Placeholder implementation for interface compliance
 * * ## Performance Considerations:
 * * The design optimizes for renewable generation patterns:
 * - Bulk copy at year start reduces memory operations
 * - Aggregation flag controls whether processing is needed
 * - Type-safe member access prevents runtime errors
 * * ## Thread Safety:
 * * Each parallel space maintains its own data structures:
 * - `yearlyValues[space]` provides isolation between parallel executions
 * - No shared state between spaces during processing
 */
template<class TraitsType, class NextT = Container::EndOfList>
class TimeSeriesValuesGenerationImpl
    : public TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType, NextT>,
                                  NextT,
                                  VCardTimeSeriesBase<TraitsType>>
{
public:
    /// @name Type Definitions
    /// @{
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesGenerationImpl<TraitsType, NextT>,
                                          NextT,
                                          VCardTimeSeriesBase<TraitsType>>;
    using VCardType = VCardTimeSeriesBase<TraitsType>;

    /// @}

    /**
     * @brief Initialize generation-specific settings from study
     *     * Caches the renewable generation aggregation setting from the study
     * configuration. This setting determines whether renewable generation
     * data should be processed as aggregated time series.
     *     * @param study The study configuration containing renewable generation settings
     */
    void initializeDerivedFromStudy(Data::Study& study)
    {
        isRenewableGenerationAggregated = study.parameters.renewableGeneration.isAggregated();
    }

    /**
     * @brief Setup generation data at the beginning of each year
     *     * For renewable generation, this method handles the bulk copying of
     * time series data if aggregation is enabled. This approach optimizes
     * performance by avoiding hourly data access.
     *     * @param year The simulation year index (0-based)
     * @param space The parallel space index for thread safety
     *     * ## Processing Logic:
     *     * 1. **Aggregation Check**: Only processes if aggregation is enabled
     * 2. **Data Access**: Uses TraitsType::areaMember for type-safe access
     * 3. **Bulk Copy**: Copies entire year's time series in one operation
     * 4. **Performance**: Avoids per-hour processing overhead
     *     * ## Data Flow:
     * ```
     * Study Year → Area Member → Time Series → Yearly Values[space]
     * ```
     */
    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        if (isRenewableGenerationAggregated)
        {
            // Type-safe access to area data using traits
            auto& holder = (BaseType::areaPtr->*TraitsType::areaMember);
            // Bulk copy of time series data for performance
            std::copy_n(holder.series.getColumn(year),
                        holder.series.timeSeries.height,
                        BaseType::yearlyValues[space].hour);
        }
    }

    /**
     * @brief Hourly processing for generation variables
     *     * For renewable generation, no specific hourly processing is required
     * since the data has already been bulk-copied during yearBeginImpl.
     * This method exists to satisfy the interface requirements.
     *     * @param state Current simulation state (unused for generation)
     * @param space The parallel space index (unused for generation)
     *     * ## Design Rationale:
     *     * Renewable generation data is typically:
     * - Pre-aggregated at the study level
     * - Copied in bulk for performance
     * - Not requiring per-hour transformations
     *     * This differs from other time series like hydro where hourly
     * processing may involve complex calculations or data transformations.
     */
    void hourForEachAreaImpl(State&, unsigned int)
    {
        // Default implementation - no specific action needed for generation
        // Values are already copied in yearBeginImpl for performance
    }

private:
    /// @name Private Members
    /// @{
    /**
     * @brief Cache for renewable generation aggregation setting
     *     * Stores whether renewable generation should be processed as aggregated
     * time series. Cached from study configuration during initialization
     * for performance.
     */
    bool isRenewableGenerationAggregated{true};
    /// @}
};

/// @}

/// @name Type Aliases for Backward Compatibility
/// @{

/**
 * @brief Legacy generation variable template using tags
 * * Provides backward compatibility for existing code using the tag-based
 * approach. Maps tags to the new traits-based implementation.
 * * @tparam Tag Generation type tag (SolarTag or WindTag)
 * @tparam NextT Next variable in the processing chain
 * * ## Usage:
 * ```cpp
 * // Legacy approach (maintained for compatibility)
 * TimeSeriesValuesGeneration<SolarTag> solar;
 * TimeSeriesValuesGeneration<WindTag> wind;
 * ```
 * * ## Migration Path:
 * For new code, prefer the direct trait-based approach:
 * ```cpp
 * // Modern approach (recommended)
 * TimeSeriesValuesSolar<> solar;
 * TimeSeriesValuesWind<> wind;
 * ```
 */
template<class Tag, class NextT = Container::EndOfList>
using TimeSeriesValuesGeneration = TimeSeriesValuesGenerationImpl<GenerationTraits<Tag>, NextT>;

/**
 * @brief Solar generation time series variable
 * * Modern, direct implementation for solar generation time series.
 * Recommended for new code as it provides better type safety and
 * clearer intent than the tag-based approach.
 * * @tparam NextT Next variable in the processing chain
 * * ## Features:
 * - Type-safe access to solar data via SolarTraits::areaMember
 * - Optimized bulk data copying for renewable generation
 * - Proper integration with simulation lifecycle
 * - Thread-safe parallel processing support
 * * ## Usage:
 * ```cpp
 * // Standalone solar variable
 * TimeSeriesValuesSolar<> solarOnly;
 * // Chained with other variables
 * TimeSeriesValuesSolar<TimeSeriesValuesWind<>> solarAndWind;
 * ```
 */
template<class NextT = Container::EndOfList>
using TimeSeriesValuesSolar = TimeSeriesValuesGenerationImpl<SolarTraits, NextT>;

/**
 * @brief Wind generation time series variable
 * * Modern, direct implementation for wind generation time series.
 * Recommended for new code as it provides better type safety and
 * clearer intent than the tag-based approach.
 * * @tparam NextT Next variable in the processing chain
 * * ## Features:
 * - Type-safe access to wind data via WindTraits::areaMember
 * - Optimized bulk data copying for renewable generation
 * - Proper integration with simulation lifecycle
 * - Thread-safe parallel processing support
 * * ## Usage:
 * ```cpp
 * // Standalone wind variable
 * TimeSeriesValuesWind<> windOnly;
 * * // Chained with other variables
 * TimeSeriesValuesWind<TimeSeriesValuesSolar<>> windAndSolar;
 * ```
 */
template<class NextT = Container::EndOfList>
using TimeSeriesValuesWind = TimeSeriesValuesGenerationImpl<WindTraits, NextT>;

/// @}

} // namespace Antares::Solver::Variable::Economy
