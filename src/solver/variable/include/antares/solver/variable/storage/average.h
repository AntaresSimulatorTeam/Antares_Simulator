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
#ifndef __SOLVER_VARIABLE_STORAGE_AVERAGE_H__
#define __SOLVER_VARIABLE_STORAGE_AVERAGE_H__

#include "averagedata.h"

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
template<class NextT = Empty, int FileFilter = Variable::Category::FileLevel::allFile>
struct Average: public NextT
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

    struct Data
    {
        double value;
        uint32_t indice;
    };

    //! Name of the filter
    static const char* Name()
    {
        return "average";
    }

public:
    Average()
    {
    }

protected:
    void initializeFromStudy(Antares::Data::Study& study)
    {
        avgdata.initializeFromStudy(study);
        // Next
        NextType::initializeFromStudy(study);
    }

    void reset()
    {
        // Reset
        avgdata.reset();
        // Next
        NextType::reset();
    }

    void merge(uint year, const IntermediateValues& rhs)
    {
        avgdata.merge(year, rhs);
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
        if (!(fileLevel & Category::FileLevel::id))
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportValues<maxHoursInAYear, VCardT, Category::hourly>(report,
                                                                                Memory::RawPointer(
                                                                                  avgdata.hourly));
                break;
            case Category::daily:
                InternalExportValues<maxDaysInAYear, VCardT, Category::daily>(report,
                                                                              avgdata.daily);
                break;
            case Category::weekly:
                InternalExportValues<maxWeeksInAYear, VCardT, Category::weekly>(report,
                                                                                avgdata.weekly);
                break;
            case Category::monthly:
                InternalExportValues<maxMonths, VCardT, Category::monthly>(report, avgdata.monthly);
                break;
            case Category::annual:
                InternalExportValues<1, VCardT, Category::annual>(report, avgdata.year);
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

    template<class VCardT>
    void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
    {
        const bool isCluster = (VCardT::categoryFileLevel & Category::FileLevel::de)
                               || (VCardT::categoryFileLevel & Category::FileLevel::de_res);
        const bool isBindingConstraint = VCardT::categoryFileLevel & Category::FileLevel::bc;
        const bool isDigest = digestLevel & Category::digestAllYears;
        if ((dataLevel & Category::DataLevel::area || dataLevel & Category::DataLevel::setOfAreas) && isDigest
            && !isCluster && !isBindingConstraint)
        {
            assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

            report.captions[0][report.data.columnIndex] = report.variableCaption;
            report.captions[1][report.data.columnIndex] = report.variableUnit;
            report.captions[2][report.data.columnIndex] = (report.variableCaption == "LOLP")
                                                            ? "values"
                                                            : "EXP";

            // Precision
            report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
              VCardT::decimal>::Value();
            // Value
            report.values[report.data.columnIndex][report.data.rowIndex] = avgdata.allYears;
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
        return avgdata.dynamicMemoryUsage() + NextType::memoryUsage();
    }

    template<template<class, int> class DecoratorT>
    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        if (Yuni::Static::Type::StrictlyEqual<DecoratorT<Empty, 0>, Average<Empty, 0>>::Yes)
        {
            return avgdata.hourly;
        }
        return NextType::template hourlyValuesForSpatialAggregate<DecoratorT>();
    }

public:
    AverageData avgdata;

private:
    template<uint Size, class VCardT, int PrecisionT>
    void InternalExportValues(SurveyResults& report, const double* array) const
    {
        assert(array);
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        // Caption
        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = (report.variableCaption == "LOLP") ? "values"
                                                                                         : "EXP";
        // Precision
        report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
          VCardT::decimal>::Value();
        // Non applicability
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        // Values
        switch (PrecisionT)
        {
        case Category::annual:
        {
            double& target = *(report.values[report.data.columnIndex]);
            target = 0;
            for (uint i = 0; i != avgdata.nbYearsCapacity; ++i)
            {
                target += array[i];
            }
            avgdata.allYears = target;
            break;
        }
        default:
            (void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);
            break;
        }

        // Next column index
        ++report.data.columnIndex;
    }

}; // class Average

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_AVERAGE_H__
