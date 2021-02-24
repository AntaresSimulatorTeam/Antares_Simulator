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
#ifndef __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__
#define __ANTARES_LIBS_STUDY_AREAS_CONSTANTS_H__

namespace Antares
{
namespace Data
{
struct CompareAreaName;

enum LinkDataIndex
{
    //! NTC Direct
    fhlNTCDirect = 0,
    //! NTC Indirect
    fhlNTCIndirect,
    //! Hurdles cost, Direct
    fhlHurdlesCostDirect,
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
