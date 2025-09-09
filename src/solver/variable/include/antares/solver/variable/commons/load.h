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
 * @file load.h
 * @brief Load time series variables for electrical demand modeling
 * This file implements load (electrical demand) time series variables using
 * the modern time series base framework. Load variables represent the electrical
 * demand that must be satisfied in each area during simulation.
 *
 * @see timeseries_base.h for the base framework
 * @see generation.h, hydro.h for other time series implementations
 */
#pragma once

#include <cstring>

#include "timeseries_base.h"

namespace Antares::Solver::Variable::Economy
{

/**
 * @brief Traits for load time series variables
 * * Defines the specific characteristics of electrical load variables:
 * - Caption: "LOAD" (as displayed in outputs and GUI)
 * - Description: Descriptive text for load generation/consumption
 * - No area member needed (load data accessed directly)
 * * ## Load vs Generation:
 * * Unlike generation variables that use areaMember pointers, load variables
 * access data directly through `area->load.series` since load is a fundamental
 * property present in all areas.
 */
struct LoadTraits
{
    /// Display name for load in outputs and GUI
    inline static constexpr std::string_view kCaption = "LOAD";
    /// Descriptive text for load time series
    inline static constexpr std::string_view kDescription = "Load generation, thoughout all MC "
                                                            "years";
};

/**
 * @brief VCard for load time series variables
 * * Provides metadata and configuration for load time series.
 * Inherits common properties from TimeSeriesTraits and adds load-specific
 * information from LoadTraits.
 * * ## Output Configuration:
 * * Load variables use the standard time series output configuration:
 * - Unit: MWh (inherited from TimeSeriesTraits)
 * - Aggregation: Sum across areas (for regional totals)
 * - Statistics: Average, StdDev, Min, Max across all Monte Carlo years
 * - Precision: All output levels (as configured in study)
 */
using VCardTimeSeriesValuesLoad = VCardTimeSeriesBase<LoadTraits>;

/**
 * @brief Load time series implementation
 * * This class implements the processing of electrical load time series data.
 * It provides efficient handling of load data that has been pre-processed
 * with DSM (Demand-Side Management) transformations.
 *
 * @tparam NextT The next variable in the processing chain
 *
 */
template<class NextT = Container::EndOfList>
class TimeSeriesValuesLoad
    : public TimeSeriesValuesBase<TimeSeriesValuesLoad<NextT>, NextT, VCardTimeSeriesValuesLoad>
{
public:
    /// @name Type Definitions
    /// @{
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesLoad<NextT>,
                                          NextT,
                                          VCardTimeSeriesValuesLoad>;

    /// @}

    /**
     * @brief Initialize load-specific settings from study
     * Load variables currently don't require specific initialization
     */
    void initializeDerivedFromStudy(Data::Study&)
    {
        // No specific initialization needed for load
        // Load data is accessed directly and doesn't require
        // special configuration like renewable generation aggregation
    }

    /**
     * @brief Setup load data at the beginning of each year
     *     * This method performs bulk copying of the load time series data
     * for the current simulation year. The data accessed here has
     * already been processed to include DSM effects.
     *
     * @param year The simulation year index (0-based)
     * @param space The parallel space index for thread safety
     *
     * ## Data Processing Note:
     *
     * **IMPORTANT**: At this point, the load values are NOT the raw values
     * from the original load time series. They have been transformed by
     * `performTransformationsBeforeLaunchingSimulation()` to include
     * Demand-Side Management (DSM) effects:
     *
     * ```
     * L* = L + DSM
     * ```
     *
     * Where:
     * - L* = Final load values (what we access here)
     * - L = Original load time series
     * - DSM = Demand-Side Management modifications
     *
     */
    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        // The current time-series
        //
        // At this point, these values are not the raw values of the load timeseries
        // see performTransformationsBeforeLaunchingSimulation()
        // L* = L + DSM
        //
        std::memcpy(BaseType::yearlyValues[space].hour,
                    BaseType::areaPtr->load.series.getColumn(year),
                    sizeof(double) * BaseType::areaPtr->load.series.timeSeries.height);
    }

    /**
     * @brief Hourly processing for load variables
     *     * For load variables, no specific hourly processing is required since
     * the data has already been bulk-copied during yearBeginImpl.
     */
    void hourForEachAreaImpl(State&, unsigned int)
    {
        // No specific action needed - values are already copied in yearBeginImpl
        // This provides optimal performance by avoiding 8760 individual operations
        // per year in favor of a single bulk memory copy.
    }
};

} // namespace Antares::Solver::Variable::Economy
