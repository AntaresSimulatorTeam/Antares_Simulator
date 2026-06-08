// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
#define __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__

#include <cmath>
#include <float.h>

#include "antares/solver/variable/storage/empty.h"

using HighPrecision = long double;

namespace Antares::Solver::Variable::R::AllYears
{
struct StdDeviation
{
public:
    static constexpr int categoryFile = Variable::Category::FileLevel::allFile;

    static const char* Name()
    {
        return "std deviation";
    }

    void initializeFromStudy(Antares::Data::Study& study)
    {
        stdDeviationHourly.assign(HOURS_PER_YEAR, 0.);
        stdDeviationDaily.assign(DAYS_PER_YEAR, 0.);
        stdDeviationWeekly.assign(WEEKS_PER_YEAR, 0.);
        stdDeviationMonthly.assign(MONTHS_PER_YEAR, 0.);

        yearsWeight = study.parameters.getYearsWeight();
        yearsWeightSum = study.parameters.getYearsWeightSum();
    }

    void reset()
    {
        stdDeviationHourly.assign(HOURS_PER_YEAR, 0.);
        stdDeviationDaily.assign(DAYS_PER_YEAR, 0.);
        stdDeviationWeekly.assign(WEEKS_PER_YEAR, 0.);
        stdDeviationMonthly.assign(MONTHS_PER_YEAR, 0.);
        stdDeviationYear = 0.;
    }

    void merge(unsigned int year, const IntermediateValues& rhs)
    {
        double pRatio = (double)yearsWeight[year] / (double)yearsWeightSum;

        unsigned int i;
        for (i = 0; i != HOURS_PER_YEAR; ++i)
        {
            stdDeviationHourly[i] += rhs.hour[i] * rhs.hour[i] * pRatio;
        }
        for (i = 0; i != DAYS_PER_YEAR; ++i)
        {
            stdDeviationDaily[i] += rhs.day[i] * rhs.day[i] * pRatio;
        }
        for (i = 0; i != WEEKS_PER_YEAR; ++i)
        {
            stdDeviationWeekly[i] += rhs.week[i] * rhs.week[i] * pRatio;
        }
        for (i = 0; i != MONTHS_PER_YEAR; ++i)
        {
            stdDeviationMonthly[i] += rhs.month[i] * rhs.month[i] * pRatio;
        }
        stdDeviationYear += rhs.year * rhs.year * pRatio;
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
    }

    template<class VCardT>
    void buildDigest(SurveyResults& /*report*/, int /*digestLevel*/, int /*dataLevel*/) const
    {
        // No digest output for std deviation.
    }

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

        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "std";

        report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
          VCardT::decimal>::Value();

        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        double* target = report.values[report.data.columnIndex];

        auto squareRootChecked = [](double d) { return d >= 0 ? std::sqrt(d) : 0.; };

        // Caller may pass either a Results<> (which exposes avgdata via accessor)
        // or an Average<> directly (where avgdata is a member). Pick whichever
        // form compiles for this S.
        const auto& avg = [&]() -> const auto&
        {
            if constexpr (requires { results.avgdata(); })
            {
                return results.avgdata();
            }
            else
            {
                return results.avgdata;
            }
        }();

        switch (PrecisionT)
        {
        case Category::hourly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = avg.hourly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::daily:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = avg.daily[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::weekly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = avg.weekly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::monthly:
        {
            for (unsigned int i = 0; i != Size; ++i)
            {
                double v = avg.monthly[i];
                double a = array[i];
                target[i] = squareRootChecked(a - v * v);
            }
        }
        break;
        case Category::annual:
        {
            const double d = *array - (double)avg.year * (double)avg.year;
            *target = squareRootChecked(d);
        }
        break;
        }

        ++report.data.columnIndex;
    }

private:
    std::vector<float> yearsWeight;
    float yearsWeightSum;

}; // class StdDeviation

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_STD_DEVIATION_H__
