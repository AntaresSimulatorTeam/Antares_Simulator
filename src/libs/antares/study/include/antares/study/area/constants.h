// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
#define __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__

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
