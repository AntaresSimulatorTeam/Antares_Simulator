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
#ifndef __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationBySTStorageGroup_H__
#define __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationBySTStorageGroup_H__

#include "../storage/results.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{
struct VCardReserveParticipationBySTStorageGroup
{
    //! Caption
    static std::string Caption()
    {
        return "SHORT TERM STORAGE GROUP PARTICIPATION TO RESERVE";
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
    typedef VCardReserveParticipationBySTStorageGroup VCardForSpatialAggregate;

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

static std::string STStorageGroupToString(Data::ShortTermStorage::Group idx)
{
    using enum Data::ShortTermStorage::Group;
    switch (idx)
    {
    case PSP_open:
        return "PSP_open";
    case PSP_closed:
        return "PSP_closed";
    case Pondage:
        return "Pondage";
    case Battery:
        return "Battery";
    case Other1:
        return "Other1";
    case Other2:
        return "Other2";
    case Other3:
        return "Other 3";
    case Other4:
        return "Other 4";
    case Other5:
        return "Other 5";
    default:
        return "<unknown>";
    }
}
} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif //__SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByDispatchablePlant_H__