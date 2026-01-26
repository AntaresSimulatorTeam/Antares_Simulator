// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_AVERAGE_H__
#define __SOLVER_VARIABLE_STORAGE_AVERAGE_H__

#include <vector>

#include "averagedata.h"

namespace Antares::Solver::Variable::R::AllYears
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
                InternalExportValues<HOURS_PER_YEAR, VCardT, Category::hourly>(report,
                                                                               avgdata.hourly);
                break;
            case Category::daily:
                InternalExportValues<DAYS_PER_YEAR, VCardT, Category::daily>(report, avgdata.daily);
                break;
            case Category::weekly:
                InternalExportValues<WEEKS_PER_YEAR, VCardT, Category::weekly>(report,
                                                                               avgdata.weekly);
                break;
            case Category::monthly:
                InternalExportValues<MONTHS_PER_YEAR, VCardT, Category::monthly>(report,
                                                                                 avgdata.monthly);
                break;
            case Category::annual:
                InternalExportValues<1, VCardT, Category::annual>(report, {avgdata.year});
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
        if ((dataLevel & Category::DataLevel::area || dataLevel & Category::DataLevel::setOfAreas)
            && isDigest && !isCluster && !isBindingConstraint)
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
            report.values[report.data.columnIndex][report.data.rowIndex] = avgdata.year;
            // Non applicability
            report.digestNonApplicableStatus[report.data.rowIndex][report.data.columnIndex]
              = *report.isCurrentVarNA;

            ++(report.data.columnIndex);
        }
        // Next
        NextType::template buildDigest<VCardT>(report, digestLevel, dataLevel);
    }

public:
    AverageData avgdata;

private:
    template<uint Size, class VCardT, int PrecisionT>
    void InternalExportValues(SurveyResults& report, const std::vector<HighPrecision>& array) const
    {
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
        case Category::hourly:
        {
            for (uint h = 0; h < HOURS_PER_YEAR; ++h)
            {
                report.values[report.data.columnIndex][h] = array[h];
            }
            break;
        }
        case Category::daily:
        {
            for (uint d = 0; d < DAYS_PER_YEAR; ++d)
            {
                report.values[report.data.columnIndex][d] = array[d];
            }
            break;
        }

        case Category::weekly:
        {
            for (uint w = 0; w < WEEKS_PER_YEAR; ++w)
            {
                report.values[report.data.columnIndex][w] = array[w];
            }
            break;
        }

        case Category::monthly:
        {
            for (uint m = 0; m < MONTHS_PER_YEAR; ++m)
            {
                report.values[report.data.columnIndex][m] = array[m];
            }
            break;
        }
        case Category::annual:
        {
            *(report.values[report.data.columnIndex]) = array[0];
            break;
        }

        default:
            logs.warning() << "Category not found for variable: " << report.variableCaption;
            break;
        }

        // Next column index
        ++report.data.columnIndex;
    }

}; // class Average

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_AVERAGE_H__
