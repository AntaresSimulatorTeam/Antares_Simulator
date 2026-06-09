// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RAW_H__
#define __SOLVER_VARIABLE_STORAGE_RAW_H__

#include <float.h>

#include <yuni/yuni.h>

#include <antares/memory/memory.h>

#include "rawdata.h"

namespace Antares::Solver::Variable::R::AllYears
{
struct Raw
{
public:
    static constexpr int categoryFile = Variable::Category::FileLevel::allFile;

    static const char* Name()
    {
        return "raw";
    }

    Raw() = default;
    ~Raw() = default;

    void initializeFromStudy(Antares::Data::Study& study);

    inline void reset()
    {
        rawdata.reset();
    }

    inline void merge(uint year, const IntermediateValues& rhs)
    {
        rawdata.merge(year, rhs);
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
                InternalExportValues<Category::hourly, HOURS_PER_YEAR, VCardT>(
                  report,
                  ::Antares::Memory::RawPointer(rawdata.hourly));
                break;
            case Category::daily:
                InternalExportValues<Category::daily, DAYS_PER_YEAR, VCardT>(report, rawdata.daily);
                break;
            case Category::weekly:
                InternalExportValues<Category::weekly, WEEKS_PER_YEAR, VCardT>(report,
                                                                               rawdata.weekly);
                break;
            case Category::monthly:
                InternalExportValues<Category::monthly, MONTHS_PER_YEAR, VCardT>(report,
                                                                                 rawdata.monthly);
                break;
            case Category::annual:
                InternalExportValues<Category::annual, 1, VCardT>(report, rawdata.year.data());
                break;
            }
        }
    }

    template<class VCardT>
    void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
    {
        if ((dataLevel & Category::DataLevel::area || dataLevel & Category::DataLevel::setOfAreas)
            && digestLevel & Category::digestAllYears)
        {
            assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

            report.captions[0][report.data.columnIndex] = report.variableCaption;
            report.captions[1][report.data.columnIndex] = report.variableUnit;
            report.captions[2][report.data.columnIndex] = "values";

            report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<
              VCardT::decimal>::Value();
            report.values[report.data.columnIndex][report.data.rowIndex] = rawdata.allYears;
            report.digestNonApplicableStatus[report.data.rowIndex][report.data.columnIndex]
              = *report.isCurrentVarNA;

            ++(report.data.columnIndex);
        }
    }

    Antares::Memory::Stored<double>::ConstReturnType hourlyForSpatialAggregate() const
    {
        return rawdata.hourly;
    }

public:
    RawData rawdata;

private:
    template<int PrecisionT, uint Size, class VCardT>
    void InternalExportValues(SurveyResults& report, const double* array) const
    {
        assert(array);
        assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

        report.captions[0][report.data.columnIndex] = report.variableCaption;
        report.captions[1][report.data.columnIndex] = report.variableUnit;
        report.captions[2][report.data.columnIndex] = "values";
        report.precision[report.data.columnIndex] = Solver::Variable::PrecisionToPrintfFormat<
          VCardT::decimal>::Value();
        report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

        if (PrecisionT == Category::annual)
        {
            rawdata.allYears = 0.;
            for (uint i = 0; i != rawdata.nbYearsCapacity; ++i)
            {
                rawdata.allYears += array[i];
            }
            *(report.values[report.data.columnIndex]) = rawdata.allYears;
        }
        else
        {
            (void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);
        }

        ++report.data.columnIndex;
    }

}; // class Raw

} // namespace Antares::Solver::Variable::R::AllYears

#include "raw.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_RAW_H__
