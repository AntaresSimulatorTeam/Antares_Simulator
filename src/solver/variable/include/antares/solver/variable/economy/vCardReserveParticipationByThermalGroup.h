/*
** Copyright 2007-2023 RTE
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
#ifndef __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByThermalGroup_H__
#define __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByThermalGroup_H__

#include "../storage/results.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardReserveParticipationByThermalGroup
{
    //! Caption
    static std::string Caption()
    {
        return "THERMAL GROUP PARTICIPATION TO RESERVE";
    }
    //! Unit
    static std::string Unit()
    {
        return "MWh";
    }

    //! The short description of the variable
    static std::string Description()
    {
        return "Reserve Participation from a group to a reserve";
    }

    //! The expected results
    typedef Results<R::AllYears::Average< // The average values throughout all years
      >>
      ResultsType;

    //! The VCard to look for for calculating spatial aggregates
    typedef VCardReserveParticipationByThermalGroup VCardForSpatialAggregate;

    enum
    {
        //! Data Level
        categoryDataLevel = Category::area,
        //! File level (provided by the type of the results)
        categoryFileLevel = ResultsType::categoryFile & (Category::id | Category::va),
        //! Precision (views)
        precision = Category::all,
        //! Indentation (GUI)
        nodeDepthForGUI = +0,
        //! Decimal precision
        decimal = 0,
        //! Number of columns used by the variable
        columnCount = Category::dynamicColumns,
        //! The Spatial aggregation
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        //! Intermediate values
        hasIntermediateValues = 1,
        //! Can this variable be non applicable (0 : no, 1 : yes)
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;

    // typedef IntermediateValues IntermediateValuesType;

}; // class VCard

static std::string thermalDispatchableGroupToString(Data::ThermalDispatchableGroup idx)
{
    switch (idx)
    {
    case 0:
        return "NUCLEAR";
    case 1:
        return "LIGNITE";
    case 2:
        return "COAL";
    case 3:
        return "GAS";
    case 4:
        return "OIL";
    case 5:
        return "MIX. FUEL";
    case 6:
        return "MISC. DTG";
    case 7:
        return "MISC. DTG 2";
    case 8:
        return "MISC. DTG 3";
    case 9:
        return "MISC. DTG 4";

    default:
        return "<unknown>";
    }
}
} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif //__SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByDispatchablePlant_H__