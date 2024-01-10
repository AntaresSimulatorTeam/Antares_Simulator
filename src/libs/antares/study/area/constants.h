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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
#define __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__

namespace Antares
{
namespace Data
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

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
