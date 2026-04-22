// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_H__

#include "minmax-data.h"

namespace Antares::Solver::Variable::R::AllYears
{
template<class NextT = Empty>
class Min;
template<class NextT = Empty>
class Max;

template<bool OpInferior, class NextT = Empty>
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

    MinMaxBase()
    {
    }

    ~MinMaxBase()
    {
    }

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
                                                                minmax.hourly.indices.data()),
                                                              fileLevel);
                break;
            case Category::daily:
                InternalExportIndices<DAYS_PER_YEAR, VCardT>(report,
                                                             minmax.daily.indices.data(),
                                                             fileLevel);
                break;
            case Category::weekly:
                InternalExportIndices<WEEKS_PER_YEAR, VCardT>(report,
                                                              minmax.weekly.indices.data(),
                                                              fileLevel);
                break;
            case Category::monthly:
                InternalExportIndices<MONTHS_PER_YEAR, VCardT>(report,
                                                               minmax.monthly.indices.data(),
                                                               fileLevel);
                break;
            case Category::annual:
                InternalExportIndices<1, VCardT>(report, minmax.annual.indices.data(), fileLevel);
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
                                                               minmax.hourly.values.data()));
                break;
            case Category::daily:
                InternalExportValues<DAYS_PER_YEAR, VCardT>(report, minmax.daily.values.data());
                break;
            case Category::weekly:
                InternalExportValues<WEEKS_PER_YEAR, VCardT>(report, minmax.weekly.values.data());
                break;
            case Category::monthly:
                InternalExportValues<MONTHS_PER_YEAR, VCardT>(report, minmax.monthly.values.data());
                break;
            case Category::annual:
                InternalExportValues<1, VCardT>(report, minmax.annual.values.data());
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
                                      const uint16_t* indices,
                                      int fileLevel);

    template<uint Size, class VCardT>
    static void InternalExportValues(SurveyResults& report, const double* values);

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

} // namespace Antares::Solver::Variable::R::AllYears

#include "minmax.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_H__
