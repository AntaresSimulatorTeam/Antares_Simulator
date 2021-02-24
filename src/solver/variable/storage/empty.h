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
#ifndef __SOLVER_VARIABLE_STORAGE_EMPTY_H__
#define __SOLVER_VARIABLE_STORAGE_EMPTY_H__

namespace Antares
{
namespace Solver
{
namespace Variable
{
struct Empty
{
public:
    //! Name of the filter
    const char* Name()
    {
        return NULL;
    }
    enum
    {
        //! The count if item in the list
        count = 0,
        categoryFile = 0,
    };

protected:
    static void initializeFromStudy(const Data::Study&)
    {
        // Does nothing
    }

    static void reset()
    {
        // Does nothing
    }

    static void merge(unsigned int /*year*/, const IntermediateValues& /*rhs*/)
    {
        // Does nothing
    }

    template<class S, class VCardT>
    static void buildSurveyReport(SurveyResults&, const S&, int, int, int)
    {
        // Does nothing
    }

    template<class VCardT>
    static void buildDigest(SurveyResults&, int, int)
    {
        // Does nothing
    }

    static Yuni::uint64 memoryUsage()
    {
        return 0;
    }

    static void EstimateMemoryUsage(Data::StudyMemoryUsage&)
    {
        // Does nothing
    }

    template<template<class, int> class DecoratorT>
    static Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate()
    {
        return Antares::Memory::Stored<double>::NullValue();
    }

}; // class Empty

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_EMPTY_H__
