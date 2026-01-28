// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_EMPTY_H__
#define __SOLVER_VARIABLE_STORAGE_EMPTY_H__

namespace Antares::Solver::Variable
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

    template<template<class, int> class DecoratorT>
    static Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate()
    {
        return Antares::Memory::Stored<double>::NullValue();
    }

}; // class Empty

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STORAGE_EMPTY_H__
