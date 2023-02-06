/*
** Copyright 2007-2018 RTE
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
#ifndef __ANTARES_LIBS_STUDY_FWD_H__
#define __ANTARES_LIBS_STUDY_FWD_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "../constants.h"
#include <map>

namespace Antares
{
class IniFile;

namespace Data
{
// Forward declarations
class Study;
class StudyMemoryUsage;
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
class BindConstList;

class StudyRuntimeInfos;

class UIRuntimeInfo;

class Correlation;

//! Name of a single area
using AreaName = Yuni::CString<ant_k_area_name_max_length, false>;
//! Name of a single link
using AreaLinkName = Yuni::CString<ant_k_area_name_max_length * 2 + 1, false>;
//! Name of a single thermal
using ClusterName = Yuni::CString<ant_k_cluster_name_max_length, false>;

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
** \brief Types of Study
*/
enum StudyMode
{
    stdmUnknown = -1,
    //! Adequation
    stdmAdequacyDraft = 0,
    //! Economic
    stdmEconomy,
    //! Adequacy (subset of the economy mode)
    stdmAdequacy,
    //! Expansion
    stdmExpansion,
    //! Max
    stdmMax

}; // enum StudyMode

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
** \brief Link Type mode
*/
enum LinkType
{
    //! Link Type defined locally
    ltLocal = 0,
    //! Link Type AC
    ltAC,

}; // enum LinkType

/*!
** \brief Types of timeSeries
**
** These values are mainly used for mask bits
*/
enum TimeSeries
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

    //! The maximum number of time-series that we can encounter
    timeSeriesCount = 7,

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
    //! The number of seeds
    seedMax,
};

enum
{
    //! A default seed for random number generators
    antaresSeedDefaultValue = 5489,
    //! Increment for RNG (generators) with a different seed
    antaresSeedIncrement = 1000000, // arbitrary value
};

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

// ... Initial reservoir levels ...
enum InitialReservoirLevels
{
    irlColdStart = 0,
    irlHotStart,
    irlUnknown,
};

/*!
** \brief Convert an Initial Reservoir Levels strategy into a text
*/
const char* InitialReservoirLevelsToCString(InitialReservoirLevels iniLevels);

/*!
** \brief Convert a text into an Initial Reservoir Levels strategy
*/
InitialReservoirLevels StringToInitialReservoirLevels(const AnyString& text);

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
    ucHeuristic = 0,
    ucMILP, // mixed integer linear problem
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

// Format of results
enum ResultFormat
{
    // Store outputs as files inside directories
    legacyFilesDirectories = 0,
    // Store outputs inside a single zip archive
    zipArchive
};

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

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
class Rules;
class TSNumberRules;
class Sets;

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

namespace Benchmarking
{
class IDurationCollector;
}

#endif // __ANTARES_LIBS_STUDY_FWD_H__
