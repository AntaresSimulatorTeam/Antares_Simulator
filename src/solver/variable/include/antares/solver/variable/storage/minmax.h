// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_H__

#include "minmax-data.h"

namespace Antares::Solver::Variable::R::AllYears
{

class MinMaxBase
{
public:
    static constexpr int categoryFile = Variable::Category::FileLevel::allFile;

    static const char* Name()
    {
        return "minmaxbase";
    }

    MinMaxBase() = default;
    ~MinMaxBase() = default;

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
                                                              minmax.hourly.indices.data(),
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
                InternalExportValues<HOURS_PER_YEAR, VCardT>(report, minmax.hourly.values.data());
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
    }

    template<class VCardT>
    void buildDigest(SurveyResults& /*report*/, int /*digestLevel*/, int /*dataLevel*/) const
    {
    }

    void reset();

    void merge(uint year, const IntermediateValues& rhs);

protected:
    MinMaxData minmax;

    bool isInf_ = true;

private:
    template<uint Size, class VCardT>
    void InternalExportIndices(SurveyResults& report, const uint16_t* indices, int fileLevel) const;

    template<uint Size, class VCardT>
    void InternalExportValues(SurveyResults& report, const double* values) const;

}; // class MinMaxBase

class Min: public MinMaxBase
{
public:
    Min()
    {
        isInf_ = true;
    }

    static const char* Name()
    {
        return "min";
    }
};

class Max: public MinMaxBase
{
public:
    Max()
    {
        isInf_ = false;
    }

    static const char* Name()
    {
        return "max";
    }
};

} // namespace Antares::Solver::Variable::R::AllYears

#include "minmax.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_H__
