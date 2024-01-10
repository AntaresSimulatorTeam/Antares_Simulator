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
#ifndef __SOLVER_VARIABLE_STORAGE_RAW_H__
#define __SOLVER_VARIABLE_STORAGE_RAW_H__

#include <yuni/yuni.h>
#include <float.h>
#include "rawdata.h"
#include <antares/memory/memory.h>

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
template<class NextT /*= Empty*/, int FileFilter /*= Variable::Category::allFile*/>
struct Raw : public NextT
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

    struct Data
    {
        double value;
        uint32_t indice;
    };

    //! Name of the filter
    static const char* Name()
    {
        return "raw";
    }

public:
    Raw()
    {
    }

    ~Raw()
    {
    }

protected:
    void initializeFromStudy(Antares::Data::Study& study);

    inline void reset()
    {
        rawdata.reset();
        // Next
        NextType::reset();
    }

    inline void merge(uint year, const IntermediateValues& rhs)
    {
        rawdata.merge(year, rhs);
        // Next
        NextType::merge(year, rhs);
    }

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        if (fileLevel & FileFilter && !(fileLevel & Category::id))
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportValues<Category::hourly, maxHoursInAYear, VCardT>(
                  report, ::Antares::Memory::RawPointer(rawdata.hourly));
                break;
            case Category::daily:
                InternalExportValues<Category::daily, maxDaysInAYear, VCardT>(report,
                                                                              rawdata.daily);
                break;
            case Category::weekly:
                InternalExportValues<Category::weekly, maxWeeksInAYear, VCardT>(report,
                                                                                rawdata.weekly);
                break;
            case Category::monthly:
                InternalExportValues<Category::monthly, maxMonths, VCardT>(report,
                                                                           rawdata.monthly);
                break;
            case Category::annual:
                InternalExportValues<Category::annual, 1, VCardT>(report, rawdata.year);
                break;
            }
        }
        // Next
        NextType::template buildSurveyReport<S, VCardT>(
          report, results, dataLevel, fileLevel, precision);
    }

    template<class VCardT>
    void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
    {
        if ((dataLevel & Category::area || dataLevel & Category::setOfAreas)
            && digestLevel & Category::digestAllYears)
        {
            assert(report.data.columnIndex < report.maxVariables
                    && "Column index out of bounds");

            report.captions[0][report.data.columnIndex] = report.variableCaption;
            report.captions[1][report.data.columnIndex] = report.variableUnit;
            report.captions[2][report.data.columnIndex] = "values";

            // Precision
            report.precision[report.data.columnIndex]
                = PrecisionToPrintfFormat<VCardT::decimal>::Value();
            // Value
            report.values[report.data.columnIndex][report.data.rowIndex] = rawdata.allYears;
            // Non applicability
            report.digestNonApplicableStatus[report.data.rowIndex][report.data.columnIndex]
                = *report.isCurrentVarNA;

            ++(report.data.columnIndex);
        }
        // Next
        NextType::template buildDigest<VCardT>(report, digestLevel, dataLevel);
    }

    uint64_t memoryUsage() const
    {
        return +sizeof(double) * maxHoursInAYear + NextType::memoryUsage();
    }

    template<template<class, int> class DecoratorT>
    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        if (Yuni::Static::Type::StrictlyEqual<DecoratorT<Empty, 0>, Raw<Empty, 0>>::Yes)
            return rawdata.hourly;
        return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
    }

public:
    RawData rawdata;

private:
    template<int PrecisionT, uint Size, class VCardT>
    void InternalExportValues(SurveyResults& report, const double* array) const
    {
        assert(array);
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        // Caption
        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "values";
        // Precision
        report.precision[report.data.columnIndex]
          = Solver::Variable::PrecisionToPrintfFormat<VCardT::decimal>::Value();
        // Non applicability
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        // Values
        if (PrecisionT == Category::annual)
        {
            rawdata.allYears = 0.;
            for (uint i = 0; i != rawdata.nbYearsCapacity; ++i)
                rawdata.allYears += array[i];
            *(report.values[report.data.columnIndex]) = rawdata.allYears;
        }
        else
            (void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);

        // Next column index
        ++report.data.columnIndex;
    }

    template<uint Size, class VCardT>
    void InternalExportValuesMC(int precision, SurveyResults& report, const double* array) const
    {
        if (not(precision & Category::annual))
            return;
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        // Caption
        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "values";
        // Precision
        report.precision[report.data.columnIndex]
          = Solver::Variable::PrecisionToPrintfFormat<VCardT::decimal>::Value();
        // Non applicability
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        (void)::memcpy(
          report.data.matrix[report.data.columnIndex], array, report.data.nbYears * sizeof(double));

        // Next column index
        ++report.data.columnIndex;
    }

}; // class Raw

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "raw.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_RAW_H__
