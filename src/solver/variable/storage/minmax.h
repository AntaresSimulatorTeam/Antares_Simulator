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
struct MinMaxBase : public NextT
{
public:
    //! Type of the net item in the list
    typedef NextT NextType;

    enum
    {
        //! The count if item in the list
        count = 1 + NextT::count,

        categoryFile = NextT::categoryFile | Variable::Category::allFile,
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
        switch (fileLevel)
        {
        case Category::mc:
            break;
        case Category::id:
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportIndices<maxHoursInAYear, VCardT>(
                  report, Memory::RawPointer(minmax.hourly), fileLevel);
                break;
            case Category::daily:
                InternalExportIndices<maxDaysInAYear, VCardT>(report, minmax.daily, fileLevel);
                break;
            case Category::weekly:
                InternalExportIndices<maxWeeksInAYear, VCardT>(report, minmax.weekly, fileLevel);
                break;
            case Category::monthly:
                InternalExportIndices<maxMonths, VCardT>(report, minmax.monthly, fileLevel);
                break;
            case Category::annual:
                InternalExportIndices<1, VCardT>(report, &minmax.annual, fileLevel);
                break;
            }
            break;
        }
        default:
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportValues<maxHoursInAYear, VCardT>(report,
                                                              Memory::RawPointer(minmax.hourly));
                break;
            case Category::daily:
                InternalExportValues<maxDaysInAYear, VCardT>(report, minmax.daily);
                break;
            case Category::weekly:
                InternalExportValues<maxWeeksInAYear, VCardT>(report, minmax.weekly);
                break;
            case Category::monthly:
                InternalExportValues<maxMonths, VCardT>(report, minmax.monthly);
                break;
            case Category::annual:
                InternalExportValues<1, VCardT>(report, &minmax.annual);
                break;
            }
        }
        }
        // Next
        NextType::template buildSurveyReport<S, VCardT>(
          report, results, dataLevel, fileLevel, precision);
    }

    void reset();

    void merge(uint year, const IntermediateValues& rhs);

    Yuni::uint64 memoryUsage() const
    {
        return
#ifdef ANTARES_SWAP_SUPPORT
          0
#else
          sizeof(double) * maxHoursInAYear
#endif
          + NextType::memoryUsage();
    }

    static void EstimateMemoryUsage(Data::StudyMemoryUsage& u)
    {
        Antares::Memory::EstimateMemoryUsage(sizeof(MinMaxData::Data), maxHoursInAYear, u, false);
        u.takeIntoConsiderationANewTimeserieForDiskOutput(true);
        NextType::EstimateMemoryUsage(u);
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
class Min : public MinMaxBase<true, NextT>
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
class Max : public MinMaxBase<false, NextT>
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
