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
#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_H__

#include "minmax-data.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace R
{
namespace AllYears
{
template<class NextT = Empty>
class Min;
template<class NextT = Empty>
class Max;

template<bool OpInferior, class NextT>
struct MinMaxBase: public NextT
{
public:
    //! Type of the net item in the list
    typedef NextT NextType;

    enum
    {
        //! The count if item in the list
        count = 1 + NextT::count,

        categoryFile = NextT::categoryFile | Variable::Category::FileLevel::allFile,
    };

    //! Name of the filter
    static const char* Name()
    {
        return "minmaxbase";
    }

public:
    MinMaxBase()
    {
    }

    ~MinMaxBase()
    {
    }

protected:
    void initializeFromStudy(Data::Study& study);

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        if (fileLevel & Category::FileLevel::id)
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportIndices<HOURS_PER_YEAR, VCardT>(report,
                                                              Memory::RawPointer(
                                                                minmax.hourly.data()),
                                                              fileLevel);
                break;
            case Category::daily:
                InternalExportIndices<DAYS_PER_YEAR, VCardT>(report,
                                                             minmax.daily.data(),
                                                             fileLevel);
                break;
            case Category::weekly:
                InternalExportIndices<WEEKS_PER_YEAR, VCardT>(report,
                                                              minmax.weekly.data(),
                                                              fileLevel);
                break;
            case Category::monthly:
                InternalExportIndices<MONTHS_PER_YEAR, VCardT>(report,
                                                               minmax.monthly.data(),
                                                               fileLevel);
                break;
            case Category::annual:
                InternalExportIndices<1, VCardT>(report, minmax.annual.data(), fileLevel);
                break;
            }
        }
        else
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportValues<HOURS_PER_YEAR, VCardT>(report,
                                                             Memory::RawPointer(
                                                               minmax.hourly.data()));
                break;
            case Category::daily:
                InternalExportValues<DAYS_PER_YEAR, VCardT>(report, minmax.daily.data());
                break;
            case Category::weekly:
                InternalExportValues<WEEKS_PER_YEAR, VCardT>(report, minmax.weekly.data());
                break;
            case Category::monthly:
                InternalExportValues<MONTHS_PER_YEAR, VCardT>(report, minmax.monthly.data());
                break;
            case Category::annual:
                InternalExportValues<1, VCardT>(report, minmax.annual.data());
                break;
            }
        }
        // Next
        NextType::template buildSurveyReport<S, VCardT>(report,
                                                        results,
                                                        dataLevel,
                                                        fileLevel,
                                                        precision);
    }

    void reset();

    void merge(uint year, const IntermediateValues& rhs);

    uint64_t memoryUsage() const
    {
        return sizeof(double) * HOURS_PER_YEAR + NextType::memoryUsage();
    }

    template<template<class> class DecoratorT>
    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
    }

protected:
    MinMaxData minmax;

private:
    template<uint Size, class VCardT>
    static void InternalExportIndices(SurveyResults& report,
                                      const MinMaxData::Data* array,
                                      int fileLevel);

    template<uint Size, class VCardT>
    static void InternalExportValues(SurveyResults& report, const MinMaxData::Data* array);

}; // class MinMaxBase

template<class NextT>
class Min: public MinMaxBase<true, NextT>
{
public:
    //! Implementation
    typedef MinMaxBase<true, NextT> MinMaxImplementationType;
    //! Type of the net item in the list
    typedef NextT NextType;

public:
    //! Name of the filter
    static const char* Name()
    {
        return "min";
    }

    enum
    {
        //! The count if item in the list
        count = MinMaxImplementationType::count,
    };
};

template<class NextT>
class Max: public MinMaxBase<false, NextT>
{
public:
    //! Implementation
    typedef MinMaxBase<false, NextT> MinMaxImplementationType;
    //! Type of the net item in the list
    typedef NextT NextType;

public:
    //! Name of the filter
    static const char* Name()
    {
        return "max";
    }

    enum
    {
        //! The count if item in the list
        count = MinMaxImplementationType::count,
    };
};

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "minmax.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_H__
