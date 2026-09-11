// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
#define __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__

#include <array>
#include <cassert>
#include <string>
#include <string_view>

namespace Antares::Data
{
struct CompareAreaName;

enum LinkParameterIndex
{
    //! Hurdles cost, Direct
    fhlHurdlesCostDirect = 0,
    //! Hurdles cost, Indirect
    fhlHurdlesCostIndirect,
    //! Impedances,
    fhlImpedances,
    //! Loop flow,
    fhlLoopFlow,
    //! P.shift min,
    fhlPShiftMinus,
    //! P.shift max,
    fhlPShiftPlus,
    //! Maximum number of items
    fhlMax,
};

/*!
** \brief Misc. Gen.
*/
enum MiscGenIndex
{
    //! CHP
    fhhCHP = 0,
    //! Bio-Mass
    fhhBioMass,
    //! BioGaz
    fhhBioGaz,
    //! Waste
    fhhWaste,
    //! GeoThermal
    fhhGeoThermal,
    //! Other */
    fhhOther,
    //! PSP - Pump
    fhhPSP,
    //! Row Balance
    fhhRowBalance,
    //! Maximum number of items
    fhhMax
};

//! Legacy component name of each misc-gen column, indexed by MiscGenIndex.
//! Single source of truth for the simulation-table component ids
//! `"{area}_{name}"`.
inline constexpr std::array<std::string_view, fhhMax> miscGenComponentNames = {
  "combined_heat_power",  // fhhCHP
  "biomass",              // fhhBioMass
  "biogas",               // fhhBioGaz
  "waste",                // fhhWaste
  "geothermal",           // fhhGeoThermal
  "other",                // fhhOther
  "pumped_storage_power", // fhhPSP
  "rest_world"            // fhhRowBalance
};

//! Suffix to append to an area id to get a misc-gen component id: `"_{name}"`.
[[nodiscard]] inline const std::string& miscGenComponentSuffix(MiscGenIndex index)
{
    static const std::array<std::string, fhhMax> suffixes = []
    {
        std::array<std::string, fhhMax> s;
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            s[i] = "_" + std::string(miscGenComponentNames[i]);
        }
        return s;
    }();
    assert(index >= fhhCHP && index < fhhMax);
    return suffixes[static_cast<std::size_t>(index)];
}

//! Full simulation-table component id of a misc-gen column: `"{area}_{name}"`.
[[nodiscard]] inline std::string miscGenComponentId(const std::string& areaId, MiscGenIndex index)
{
    return areaId + miscGenComponentSuffix(index);
}

enum ReservesIndex
{
    //! Primary reserve
    fhrPrimaryReserve = 0,
    //! Strategic reserve
    fhrStrategicReserve,
    //! DSM
    fhrDSM,
    //! D-1
    fhrDayBefore,
    //! Maximum number of items
    fhrMax,
};

enum AreaNodalOptimization
{
    //! Non Dispatchable power
    anoNonDispatchPower = 1,
    //! Dispatchable Hydro power
    anoDispatchHydroPower = 2,
    //! Other dispatchable power
    anoOtherDispatchPower = 4,

    //! All features
    anoAll = anoNonDispatchPower | anoDispatchHydroPower | anoOtherDispatchPower,
};

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
