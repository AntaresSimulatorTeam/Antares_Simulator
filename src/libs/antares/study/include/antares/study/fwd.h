/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_FWD_H__
#define __ANTARES_LIBS_STUDY_FWD_H__

#include <map>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "antares/antares/antares.h"
#include "antares/antares/constants.h"

namespace Antares
{
class IniFile;

namespace Data
{
// Forward declarations
class Study;
class StudyLoadOptions;
class Area;
class AreaLink;
class AreaList;
class AreaUI;
class ThermalCluster;
class RenewableCluster;

class AreaScratchpad;

class Parameters;

class BindingConstraint;
class BindingConstraintsRepository;

class StudyRuntimeInfos;

class UIRuntimeInfo;

class Correlation;

//! Name of a single area
using AreaName = Yuni::CString<ant_k_area_name_max_length, false>;
//! Name of a single link
using AreaLinkName = Yuni::CString<ant_k_area_name_max_length * 2 + 1, false>;
//! Name of a single thermal
using ClusterName = std::string;

using ConstraintName = Yuni::CString<ant_k_constraint_name_max_length, false>;

using RulesScenarioName = Yuni::CString<64, false>;

//! Name mapping
using AreaNameMapping = std::map<AreaName, AreaName>;

/*!
** \brief Study's Consistency errors
*/
enum StudyError
{
    stErrNone = 0,
    //! startTime, finalHour = 0..8760
    stErrHourOutOfBounds = 1,
    //! The final hour must be a multiple of 168
    stErrFinalHourMustBeAMultipleOf168,
    //! A given day of the week is invalid
    stErrInvalidDayOfWeek,
    //! Too much areas (>300)
    stErrTooMuchAreas,
    //! Too much interconnections (>5000)
    stErrTooMuchInterconnections,
    //! Too much binding constaints
    stErrTooMuchBindingConstraints,
    /**/
};

/*!
** \brief Types of Simulation
*/
enum class SimulationMode
{
    //! Economic
    Economy,
    //! Adequacy (subset of the economy mode)
    Adequacy,
    //! Expansion
    Expansion,
    //! Unknown simulation mode
    Unknown
};

/*
** \brief Simplex optimizations
*/
enum SimplexOptimization
{
    sorUnknown = 0,
    sorDay,
    sorWeek,

}; // enum SimplexOptimization

/*!
** \brief Transmission capacities mode (applies to all links)
*/
enum class GlobalTransmissionCapacities
{
    //! Transmission capacities are enabled
    localValuesForAllLinks = 0,
    //! Transmission capacities are ignored (set to null)
    nullForAllLinks,
    //! Transmission capacities are infinite (aka Copper Plate)
    infiniteForAllLinks,
    //! Transmission capacities are ignored only for physical links
    //! Global property only, cannot be used for individual links
    nullForPhysicalLinks,
    //! Transmission capacities are infinite only for physical links (not virtual)
    //! Global property only, cannot be used for individual links
    infiniteForPhysicalLinks
}; // enum TransmissionCapacities

std::string GlobalTransmissionCapacitiesToString(
  GlobalTransmissionCapacities transmissionCapacities);
std::string GlobalTransmissionCapacitiesToString_Display(
  GlobalTransmissionCapacities transmissionCapacities);
bool stringToGlobalTransmissionCapacities(const AnyString& value,
                                          GlobalTransmissionCapacities& out);

/*!
** \brief Transmission capacities mode (applies to individual links)
*/
enum class LocalTransmissionCapacities
{
    enabled,
    null,
    infinite
};

std::string transmissionCapacitiesToString(const LocalTransmissionCapacities& tc);

/*!
** \brief Asset Type mode
*/
enum AssetType
{
    //! Asset Type AC
    atAC = 0,
    //! Asset Type DC
    atDC,
    //! Asset Type gas
    atGas,
    //! Asset Type virtual
    atVirt,
    //! Asset Type Other (gas, etc)
    atOther,

}; // enum AssetType

std::string assetTypeToString(const AssetType& assetType);

/*!
** \brief Style Type mode
*/
enum StyleType
{
    //! Style Type plain
    stPlain = 0,
    //! Style Type dot
    stDot,
    //! Style Type dash
    stDash,
    //! Style Type dot & dash
    stDotDash,

}; // enum StyleType

std::string styleToString(const StyleType& style);

/*!
** \brief Types of timeSeries
**
** These values are mainly used for mask bits
*/
static const unsigned int timeSeriesCount = 7;

enum TimeSeriesType : unsigned int
{
    //! TimeSeries : Load
    timeSeriesLoad = 1,
    //! TimeSeries : Hydro
    timeSeriesHydro = 2,
    //! TimeSeries : Wind
    timeSeriesWind = 4,
    //! TimeSeries : Thermal
    timeSeriesThermal = 8,
    //! TimeSeries : Solar
    timeSeriesSolar = 16,
    //! TimeSeries : Renewable
    timeSeriesRenewable = 32,
    //! TimeSeries : Renewable
    timeSeriesTransmissionCapacities = 64,
    // ***********************************************************************
    // Please update the constant timeSeriesCount if you add / remove an item
    // ***********************************************************************
}; // enum TimeSeries

template<int T>
struct TimeSeriesBitPatternIntoIndex;

template<>
struct TimeSeriesBitPatternIntoIndex<1>
{
    enum
    {
        value = 0
    };
};

template<>
struct TimeSeriesBitPatternIntoIndex<2>
{
    enum
    {
        value = 1
    };
};

template<>
struct TimeSeriesBitPatternIntoIndex<4>
{
    enum
    {
        value = 2
    };
};

template<>
struct TimeSeriesBitPatternIntoIndex<8>
{
    enum
    {
        value = 3
    };
};

template<>
struct TimeSeriesBitPatternIntoIndex<16>
{
    enum
    {
        value = 4
    };
};

template<>
struct TimeSeriesBitPatternIntoIndex<32>
{
    enum
    {
        value = 5
    };
};

template<int T>
struct TimeSeriesToCStr;

template<>
struct TimeSeriesToCStr<1>
{
    static const char* Value()
    {
        return "load";
    }
};

template<>
struct TimeSeriesToCStr<2>
{
    static const char* Value()
    {
        return "hydro";
    }
};

template<>
struct TimeSeriesToCStr<4>
{
    static const char* Value()
    {
        return "wind";
    }
};

template<>
struct TimeSeriesToCStr<8>
{
    static const char* Value()
    {
        return "thermal";
    }
};

template<>
struct TimeSeriesToCStr<16>
{
    static const char* Value()
    {
        return "solar";
    }
};

template<>
struct TimeSeriesToCStr<32>
{
    static const char* Value()
    {
        return "renewable";
    }
};

enum SeedIndex
{
    //! The seed for wind
    seedTsGenWind = 0,
    //! The seed for load
    seedTsGenLoad,
    //! The seed for hydro
    seedTsGenHydro,
    //! The seed for thermal
    seedTsGenThermal,
    //! The seed for solar
    seedTsGenSolar,
    //! The seed used for time-series numbers
    seedTimeseriesNumbers,
    //! Seed - unsupplied energy costs
    seedUnsuppliedEnergyCosts,
    //! Seed - Spilled energy costs
    seedSpilledEnergyCosts,
    //! Seed - thermal costs
    seedThermalCosts,
    //! Seed - Hydro costs
    seedHydroCosts,
    //! Seed - Hydro management
    seedHydroManagement,
    //! The seed for links
    seedTsGenLinks,
    //! The number of seeds
    seedMax,
};

//! A default seed for random number generators
constexpr unsigned antaresSeedDefaultValue = 5489;
//! Increment for RNG (generators) with a different seed
constexpr unsigned antaresSeedIncrement = 1000000; // arbitrary value

/*!
** \brief Convert a seed into a string
*/
const char* SeedToCString(SeedIndex seed);

/*!
** \brief Seed to CString ID
**
** Mainly used for saving / loading into generaldata.ini
*/
const char* SeedToID(SeedIndex seed);

// ... Hydro heuristic policy ...
enum HydroHeuristicPolicy
{
    hhpAccommodateRuleCurves = 0,
    hhpMaximizeGeneration,
    hhpUnknown,
};

HydroHeuristicPolicy StringToHydroHeuristicPolicy(const AnyString& text);

const char* HydroHeuristicPolicyToCString(HydroHeuristicPolicy hhPolicy);

// ... Hydro Pricing ...
enum HydroPricingMode
{
    hpHeuristic = 0,
    hpMILP, // mixed integer linear problem
    hpUnknown,
};

/*!
** \brief Convert a hyfro pricing mode into a text
*/
const char* HydroPricingModeToCString(HydroPricingMode hpm);

/*!
** \brief Convert a text into a hydro pricing mode
*/
HydroPricingMode StringToHydroPricingMode(const AnyString& text);

// ... Power fluctuations ...
enum PowerFluctuations
{
    lssFreeModulations = 0,
    lssMinimizeRamping,
    lssMinimizeExcursions,
    lssUnknown,
};

/*!
** \brief Convert a local shedding strategy into a text
*/
const char* PowerFluctuationsToCString(PowerFluctuations fluctuations);

/*!
** \brief Convert a text into a local shedding strategy
*/
PowerFluctuations StringToPowerFluctuations(const AnyString& text);

enum SheddingPolicy
{
    shpShavePeaks = 0,
    shpMinimizeDuration,
    shpUnknown,
};

/*!
** \brief Convert a global shedding policy into a text
*/
const char* SheddingPolicyToCString(SheddingPolicy strategy);

/*!
** \brief Convert a text into a global shedding policy
*/
SheddingPolicy StringToSheddingPolicy(const AnyString& text);

enum UnitCommitmentMode
{
    //! Heuristic in which 2 LP problems are solved
    //! No explicit modelling for the number of ON/OFF units
    ucHeuristicFast = 0,
    //! Heuristic in which 2 LP problems are solved
    //! Explicit modelling for the number of ON/OFF units
    ucHeuristicAccurate,
    //! A single MILP problem is solved, with explicit modelling
    //! for the number of ON/OFF units
    ucMILP,
    //! Unknown mode, mainly used for error handling
    ucUnknown,
};

/*!
** \brief Convert a unit commitment mode into a text
*/
const char* UnitCommitmentModeToCString(UnitCommitmentMode ucommitment);

/*!
** \brief Convert a text into a unit commitment mode
*/
UnitCommitmentMode StringToUnitCommitmentMode(const AnyString& text);

enum NumberOfCoresMode
{
    ncMin = 0,
    ncLow,
    ncAvg,
    ncHigh,
    ncMax,
    ncUnknown
};

/*!
** \brief Convert a numbers of cores mode into a text
*/
const char* NumberOfCoresModeToCString(NumberOfCoresMode ncores);

/*!
** \brief Convert a text into a nb of cores mode
*/
NumberOfCoresMode StringToNumberOfCoresMode(const AnyString& text);

/*
 * Renewable generation modelling
 */
enum RenewableGenerationModelling
{
    rgAggregated = 0, // Default
    rgClusters,       // Using renewable clusters
    rgUnknown,
};

/*!
** \brief Convert a renewable generation modelling into a text
*/
const char* RenewableGenerationModellingToCString(RenewableGenerationModelling rgModelling);

/*!
** \brief Convert a text into a renewable generation modelling
*/
RenewableGenerationModelling StringToRenewableGenerationModelling(const AnyString& text);

// ------------------------
// MPS export status
// ------------------------
enum class mpsExportStatus : int
{
    NO_EXPORT = 0,
    EXPORT_FIRST_OPTIM = 1,
    EXPORT_SECOND_OPTIM = 2,
    EXPORT_BOTH_OPTIMS = 3,
    UNKNOWN_EXPORT = 4
};

std::string mpsExportStatusToString(const mpsExportStatus& mps_export_status);
mpsExportStatus stringToMPSexportStatus(const AnyString& value);

} // namespace Data
} // namespace Antares

namespace Antares::Data::ScenarioBuilder
{
class Rules;
class TSNumberRules;
class Sets;

} // namespace Antares::Data::ScenarioBuilder

namespace Benchmarking
{
class DurationCollector;
}

#endif // __ANTARES_LIBS_STUDY_FWD_H__
