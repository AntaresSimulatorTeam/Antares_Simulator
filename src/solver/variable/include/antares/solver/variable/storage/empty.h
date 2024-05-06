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

    static uint64_t memoryUsage()
    {
        return 0;
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
