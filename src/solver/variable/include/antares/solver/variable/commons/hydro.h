// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
 */
struct HydroTraits
{
    /// Display name for hydro ROR in outputs and GUI
    inline static constexpr std::string_view kCaption = "H. ROR";
    /// Descriptive text for hydro generation time series
    inline static constexpr std::string_view kDescription = "Hydro generation, thoughout all MC "
                                                            "years";
};

using VCardTimeSeriesValuesHydro = VCardTimeSeriesBase<HydroTraits>;

/**
 * @brief Hydro ROR time series implementation
 * * This class implements the processing of hydro run-of-river time series data.
 * It handles the dynamic selection of time series and hourly data extraction
 * specific to hydro modeling requirements.
 */
class TimeSeriesValuesHydro
    : public TimeSeriesValuesBase<TimeSeriesValuesHydro, VCardTimeSeriesValuesHydro>
{
public:
    using BaseType = TimeSeriesValuesBase<TimeSeriesValuesHydro, VCardTimeSeriesValuesHydro>;

    void initializeDerivedFromStudy(Data::Study&)
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
     * Stores pointers to the selected time series for each parallel execution
     */
    std::vector<Matrix<>::ColumnType*> fatalValues;

    /// @}
};

} // namespace Antares::Solver::Variable::Economy
