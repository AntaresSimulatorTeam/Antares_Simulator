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
 * * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/**
 * @file hydro.h* @brief Hydro ROR (Run-of-River) time series variables for hydroelectric power
 * @file hydro.h
 * @brief Hydro ROR (Run-of-River) time series variables for hydroelectric power modeling
 *
 * This file implements hydro run-of-river time series variables using the modern
 * time series base framework. Hydro variables represent the electricity generation
 * from run-of-river hydroelectric installations.
 *
 * ## Key Features:
 *
 * - **Dynamic Time Series Access**: Handles variable time series indices per year
 * - **ROR Modeling**: Specific implementation for run-of-river hydro generation
 * - **Modern Memory Management**: Uses std::vector instead of raw pointers
 * - **Hourly Processing**: Granular access to time series data per simulation hour
 *
 * ## Hydro-Specific Behavior:
 *
 * Hydro variables differ from other time series types:
 * - **Dynamic Series Selection**: Each year may use a different time series (nbchro)
 * - **Hourly Data Access**: Values are extracted hour by hour during simulation
 * - **ROR Specialization**: Focused on run-of-river rather than reservoir hydro
 * - **Series Index Mapping**: Uses getSeriesIndex() to determine which series to use
 *
 * ## Data Processing Flow:
 *
 * ```
 * Year Begin: area->hydro.series->ror.getSeriesIndex(year) → time series selection
 *       ↓
 * Hour Processing: (*fatalValues[space])[hourInYear] → hourly value extraction
 *       ↓
 * Storage: yearlyValues[space][hourInYear] ← accumulation for statistics
 * ```
 *
 * ## ROR vs Reservoir Hydro:
 *
 * This implementation focuses on **Run-of-River (ROR)** hydro generation:
 * - **ROR**: Flows directly through turbines, minimal storage, weather-dependent
 * - **Reservoir**: Large storage capacity, optimizable dispatch (handled elsewhere)
 *
 * @see timeseries_base.h for the base framework
 * @see generation.h, load.h for other time series implementations
 */
#pragma once

#include <vector>

#include "timeseries_base.h"

namespace Antares::Solver::Variable::Economy
{

/**
 * @brief Traits for hydro ROR time series variables
 * * Defines the specific characteristics of hydro run-of-river variables:
 * - Caption: "H. ROR" (Hydro Run-of-River, as displayed in outputs)
 * - Description: Descriptive text for hydro generation across Monte Carlo years
 * - No area member needed (hydro data accessed through specialized path)
 * * ## Hydro Data Access Pattern:
 * * Unlike generation variables that use direct area members, hydro variables
 * access data through a more complex path:
 * ```cpp
 * area->hydro.series->ror.timeSeries.entry[seriesIndex]
 * ```
 * * This reflects the more sophisticated hydro modeling system that includes:
 * - Multiple time series per area (different hydrological scenarios)
 * - ROR vs reservoir distinction
 * - Dynamic series selection based on Monte Carlo draw
 */
struct HydroTraits
{
    /// Display name for hydro ROR in outputs and GUI
    inline static constexpr std::string_view kCaption = "H. ROR";
    /// Descriptive text for hydro generation time series
    inline static constexpr std::string_view kDescription = "Hydro generation, thoughout all MC "
                                                            "years";
};

/**
 * @brief VCard for hydro ROR time series variables
 * * Provides metadata and configuration for hydro ROR time series.
 * Inherits common properties from TimeSeriesTraits and adds hydro-specific
 * information from HydroTraits.
 * * ## Output Configuration:
 * * Hydro variables use the standard time series output configuration:
 * - Unit: MWh (inherited from TimeSeriesTraits)
 * - Aggregation: Sum across areas (for regional hydro totals)
 * - Statistics: Average, StdDev, Min, Max across all Monte Carlo years
 * - Caption: "H. ROR" to distinguish from reservoir hydro
 * * ## Spatial Aggregation:
 * * When aggregating across multiple areas, hydro ROR values are summed:
 * - Regional total = sum of all area ROR generation
 * - Maintains physical meaning (total renewable hydro generation)
 * - Consistent with other generation types (solar, wind)
 */
using VCardTimeSeriesValuesHydro = VCardTimeSeriesBase<HydroTraits>;

/**
 * @brief Hydro ROR time series implementation
 * * This class implements the processing of hydro run-of-river time series data.
 * It handles the dynamic selection of time series and hourly data extraction
 * specific to hydro modeling requirements.
 *
 * @tparam NextT The next variable in the processing chain
 *
 * ## Processing Characteristics:
 *
 * ### Year Begin Processing:
 * - Dynamic time series selection based on Monte Carlo scenario
 * - Pointer setup for efficient hourly access
 * - No bulk data copying (accessed hourly for flexibility)
 *
 * ### Hourly Processing:
 * - Direct extraction from selected time series
 * - Hour-by-hour accumulation into yearly values
 * - Maintains temporal resolution for detailed analysis
 *
 * ## Data Access Pattern:
 *
 * ```cpp
 * // Year begin: Setup time series pointer
 * auto& ror = area->hydro.series->ror;
 * unsigned int seriesIndex = ror.getSeriesIndex(year);
 * fatalValues[space] = &(ror.timeSeries.entry[seriesIndex]);
 *
 * // Hourly: Extract specific hour value
 * yearlyValues[space][hourInYear] = (*fatalValues[space])[hourInYear];
 * ```
 *
 * ## Memory Management:
 *
 * Modern C++ approach using std::vector:
 * - **Before**: Raw pointer array with manual new[]/delete[]
 * - **After**: std::vector with automatic memory management
 * - **Benefits**: Exception safety, automatic cleanup, bounds checking in debug
 *
 * ## Thread Safety:
 *
 * Each parallel space maintains isolated data:
 * - `fatalValues[space]` provides per-thread time series pointers
 * - `yearlyValues[space]` provides per-thread value storage
 * - No shared mutable state during simulation
 *
 * ## Performance Considerations:
 *
 * - **Pointer Indirection**: Minimal overhead for hourly access
 * - **Memory Layout**: Respects existing time series organization
 * - **Cache Efficiency**: Sequential access within each hour
 * - **Flexibility**: Allows for different time series per year/scenario
 */
template<class NextT = Container::EndOfList>
class TimeSeriesValuesHydro
    : public TimeSeriesValuesBase<TimeSeriesValuesHydro<NextT>, NextT, VCardTimeSeriesValuesHydro>
{
public:
    /// @name Type Definitions
    /// @{
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesHydro<NextT>,
                                          NextT,
                                          VCardTimeSeriesValuesHydro>;

    /// @}

    /**
     * @brief Initialize hydro-specific data structures from study
     *     * Sets up the vector for storing time series pointers. This modern
     * C++ approach replaces the previous manual memory management with
     * automatic RAII-based resource handling.
     *     * @param study The study configuration containing parallel execution settings
     *     * ## Memory Management Evolution:
     *     * **Previous approach:**
     * ```cpp
     * pFatalValues = new Matrix<>::ColumnType*[pNbYearsParallel];
     * // Manual initialization and cleanup required
     * ~TimeSeriesValuesHydro() { delete[] pFatalValues; }
     * ```
     *     * **Current approach:**
     * ```cpp
     * fatalValues.resize(nbYearsParallel, nullptr);
     * // Automatic cleanup via std::vector destructor
     * ```
     *     * ## Benefits of Modern Approach:
     * - **Exception Safety**: No memory leaks if initialization throws
     * - **Automatic Cleanup**: Destructor handles resource deallocation
     * - **Debug Support**: Bounds checking available in debug builds
     * - **Standard Library**: Consistent with modern C++ practices
     */
    void initializeDerivedFromStudy(Data::Study& study)
    {
        // Initialize the vector for fatal values (modern C++ approach)
        // This replaces the previous manual memory management with RAII
        fatalValues.resize(BaseType::nbYearsParallel, nullptr);
    }

    /**
     * @brief Setup hydro time series selection at the beginning of each year
     *     * This method determines which time series to use for the current simulation
     * year and sets up efficient access for hourly processing. Each Monte Carlo
     * year may use a different hydrological scenario.
     *
     * @param year The simulation year index (0-based)
     * @param space The parallel space index for thread safety
     *
     * ## Time Series Selection Logic:
     *
     * 1. **Access ROR Data**: Get run-of-river time series collection
     * 2. **Determine Series**: Use getSeriesIndex() to map year to series
     * 3. **Setup Pointer**: Store reference for efficient hourly access
     *
     * ## Data Flow:
     *
     * ```
     * Simulation Year (0-based)
     *        ↓
     * getSeriesIndex(year) → Maps to specific hydrological scenario
     *        ↓
     * ror.timeSeries.entry[nbchro] → Actual time series data
     *        ↓
     * fatalValues[space] → Cached pointer for hourly access
     * ```
     *
     * ## Why "Fatal Values"?
     *
     * The name `fatalValues` is historical and refers to the fact that these
     * values are "fatal" (fixed/predetermined) for the simulation - they
     * represent natural water flows that cannot be controlled or optimized,
     * unlike reservoir hydro which can be dispatched strategically.
     *
     * ## Thread Safety:
     *
     * Each parallel space gets its own pointer, ensuring that concurrent
     * execution threads don't interfere with each other's data access.
     */
    void yearBeginImpl(unsigned int year, unsigned int space)
    {
        // Access the run-of-river hydro time series data
        auto& ror = BaseType::areaPtr->hydro.series->ror;
        // Determine which time series to use for this simulation year
        // This allows different hydrological scenarios across Monte Carlo years
        const unsigned int nbchro = ror.getSeriesIndex(year);
        // Cache pointer to the selected time series for efficient hourly access
        // This avoids repeated index calculations during hourly processing
        fatalValues[space] = &(ror.timeSeries.entry[nbchro]);
    }

    /**
     * @brief Extract hourly hydro generation values during simulation
     *     * This method extracts the specific hourly value from the selected hydro
     * time series and stores it in the yearly values array for statistics
     * computation and output generation.
     *
     * @param state Current simulation state containing hour information
     * @param space The parallel space index for thread safety
     *
     * ## Hourly Processing Logic:
     *
     * 1. **Hour Identification**: Extract current hour from simulation state
     * 2. **Value Lookup**: Access pre-selected time series at current hour
     * 3. **Value Storage**: Store in yearly values for statistics computation
     *
     * ## Data Access Pattern:
     *
     * ```cpp
     * // Direct array access using cached pointer:
     * double hydroValue = (*fatalValues[space])[state.hourInTheYear];
     *
     * // Store for statistics computation:
     * yearlyValues[space][state.hourInTheYear] = hydroValue;
     * ```
     *
     * ## Performance Characteristics:
     *
     * - **Single Pointer Dereference**: Minimal overhead per hour
     * - **Direct Array Access**: O(1) time complexity
     * - **Sequential Processing**: Good cache locality within each hour
     * - **No Calculations**: Pure data movement, no transformations
     *
     * ## Hour Range:
     *
     * - `state.hourInTheYear` ranges from 0 to 8759 (8760 hours per year)
     * - Handles both standard years (8760h) and leap years where applicable
     * - Direct mapping to time series array indices
     *
     * ## Comparison with Other Time Series:
     *
     * - **Load**: Bulk copy entire year in yearBegin, no-op in hourly
     * - **Generation**: Bulk copy if aggregated, no-op in hourly
     * - **Hydro**: No bulk copy, individual hourly processing (current)
     *
     * The hydro approach provides maximum flexibility for future enhancements
     * like hourly-dependent calculations or real-time hydro optimization.
     */
    void hourForEachAreaImpl(State& state, unsigned int space)
    {
        // Extract the hydro generation value for the current hour
        // Uses the time series pointer cached during yearBeginImpl
        BaseType::yearlyValues[space][state.hourInTheYear] = (*fatalValues[space])
          [state.hourInTheYear];
    }

private:
    /// @name Private Members
    /// @{
    /**
     * @brief Cached pointers to time series data for each parallel space
     *     * Stores pointers to the selected time series for each parallel execution
     * space. This modern C++ approach using std::vector replaces the previous
     * manual memory management with raw pointer arrays.
     *     * ## Array Organization:
     * - Index: Parallel space (0 to nbYearsParallel-1)
     * - Value: Pointer to Matrix<>::ColumnType (time series array)
     * - Lifetime: Set in yearBeginImpl, used in hourForEachAreaImpl
     *     * ## Memory Safety:
     * - **Non-owning pointers**: Data owned by hydro.series structure
     * - **Automatic cleanup**: std::vector destructor handles deallocation
     * - **Exception safe**: No manual delete[] required
     *     * ## Why Vector of Pointers?
     *
     * Each parallel space may process different Monte Carlo years simultaneously,
     * potentially requiring different time series. The vector provides:
     * - **Isolation**: Each space has independent time series access
     * - **Performance**: Direct pointer access avoids repeated index calculations
     * - **Flexibility**: Different spaces can use different series concurrently
     */
    std::vector<Matrix<>::ColumnType*> fatalValues;

    /// @}
};

} // namespace Antares::Solver::Variable::Economy
