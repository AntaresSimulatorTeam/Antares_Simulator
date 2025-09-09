/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#ifndef __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
#define __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__

#include <cmath>
#include <float.h>

using HighPrecision = long double;

namespace Antares::Solver::Variable::R::AllYears
{
template<class NextT = Empty, int FileFilter = Variable::Category::FileLevel::allFile>
struct StdDeviation: public NextT
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
        return "std deviation";
    }

protected:
    void initializeFromStudy(Antares::Data::Study& study)
    {
        stdDeviationHourly.assign(HOURS_PER_YEAR, 0.);
        stdDeviationDaily.assign(DAYS_PER_YEAR, 0.);
        stdDeviationWeekly.assign(WEEKS_PER_YEAR, 0.);
        stdDeviationMonthly.assign(MONTHS_PER_YEAR, 0.);
        // Next
        NextType::initializeFromStudy(study);

        yearsWeight = study.parameters.getYearsWeight();
        yearsWeightSum = study.parameters.getYearsWeightSum();
    }

    void reset()
    {
        // Reset
        stdDeviationHourly.assign(HOURS_PER_YEAR, 0.);
        stdDeviationDaily.assign(DAYS_PER_YEAR, 0.);
        stdDeviationWeekly.assign(WEEKS_PER_YEAR, 0.);
        stdDeviationMonthly.assign(MONTHS_PER_YEAR, 0.);
        stdDeviationYear = 0.;
        // Next
        NextType::reset();
    }

    void merge(unsigned int year, const IntermediateValues& rhs)
    {
        // Ratio take into account MC year weight
        double pRatio = (double)yearsWeight[year] / (double)yearsWeightSum;

        unsigned int i;
        // StdDeviation value for each hour throughout all years
        for (i = 0; i != HOURS_PER_YEAR; ++i)
        {
            stdDeviationHourly[i] += rhs.hour[i] * rhs.hour[i] * pRatio;
        }
        // StdDeviation value for each day throughout all years
        for (i = 0; i != DAYS_PER_YEAR; ++i)
        {
            stdDeviationDaily[i] += rhs.day[i] * rhs.day[i] * pRatio;
        }
        // StdDeviation value for each week throughout all years
        for (i = 0; i != WEEKS_PER_YEAR; ++i)
        {
            stdDeviationWeekly[i] += rhs.week[i] * rhs.week[i] * pRatio;
        }
        // StdDeviation value for each month throughout all years
        for (i = 0; i != MONTHS_PER_YEAR; ++i)
        {
            stdDeviationMonthly[i] += rhs.month[i] * rhs.month[i] * pRatio;
        }
        // StdDeviation value throughout all years
        stdDeviationYear += rhs.year * rhs.year * pRatio;

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
                InternalExportValues<S, HOURS_PER_YEAR, VCardT, Category::hourly>(
                  report,
                  results,
                  stdDeviationHourly.data());
                break;
            case Category::daily:
                InternalExportValues<S, DAYS_PER_YEAR, VCardT, Category::daily>(
                  report,
                  results,
                  stdDeviationDaily.data());
                break;
            case Category::weekly:
                InternalExportValues<S, WEEKS_PER_YEAR, VCardT, Category::weekly>(
                  report,
                  results,
                  stdDeviationWeekly.data());
                break;
            case Category::monthly:
                InternalExportValues<S, MONTHS_PER_YEAR, VCardT, Category::monthly>(
                  report,
                  results,
                  stdDeviationMonthly.data());
                break;
            case Category::annual:
                InternalExportValues<S, 1, VCardT, Category::annual>(report,
                                                                     results,
                                                                     &stdDeviationYear);
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

public:
    std::vector<HighPrecision> stdDeviationMonthly;
    std::vector<HighPrecision> stdDeviationWeekly;
    std::vector<HighPrecision> stdDeviationDaily;
    std::vector<HighPrecision> stdDeviationHourly;
    HighPrecision stdDeviationYear = 0;

private:
    template<class S, unsigned int Size, class VCardT, int PrecisionT>
    void InternalExportValues(SurveyResults& report,
                              const S& results,
                              const HighPrecision* array) const
    {
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        // Caption
        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "std";

        // Precision
        report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
          VCardT::decimal>::Value();

        // Non applicability
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        // Values
        double* target = report.values[report.data.columnIndex];
        // A mere copy

        auto squareRootChecked = [](double d) { return d >= 0 ? std::sqrt(d) : 0.; };

        switch (PrecisionT)
        {
        case Category::hourly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = results.avgdata.hourly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::daily:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = results.avgdata.daily[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::weekly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = results.avgdata.weekly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::monthly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = results.avgdata.monthly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::annual:
        {
            const double d = *array - results.avgdata.allYears * results.avgdata.allYears;
            *target = squareRootChecked(d);
        }
        break;
        }

        // Next column index
        ++report.data.columnIndex;
    }

    template<class S, unsigned int Size, class VCardT, int PrecisionT, class A>
    void InternalExportValuesMC(SurveyResults& report, const S& /*results*/, const A& array) const
    {
        if (not(PrecisionT & Category::annual))
        {
            return;
        }
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        // Caption
        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "std";

        // Precision
        report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
          VCardT::decimal>::Value();

        // Non applicability
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        (void)::memcpy(report.data.matrix[report.data.columnIndex], array, Size * sizeof(double));

        // Next column index
        ++report.data.columnIndex;
    }

private:
    std::vector<float> yearsWeight;
    float yearsWeightSum;

}; // class StdDeviation

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
