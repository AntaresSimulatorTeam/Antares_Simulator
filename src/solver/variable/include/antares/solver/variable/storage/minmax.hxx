// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MIN_HXX__
#define __SOLVER_VARIABLE_STORAGE_MIN_HXX__

namespace Antares::Solver::Variable::R::AllYears
{

inline void MinMaxBase::initializeFromStudy(Data::Study& /*study*/)
{
}

inline void MinMaxBase::reset()
{
    if (isInf_)
    {
        minmax.resetInf();
    }
    else
    {
        minmax.resetSup();
    }
}

inline void MinMaxBase::merge(uint year, const IntermediateValues& rhs)
{
    if (isInf_)
    {
        minmax.mergeInf(year, rhs);
    }
    else
    {
        minmax.mergeSup(year, rhs);
    }
}

template<uint Size, class VCardT>
void MinMaxBase::InternalExportIndices(SurveyResults& report,
                                       const uint16_t* indices,
                                       int fileLevel) const
{
    assert(indices);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (isInf_ ? "min" : "max");

    int recommendedPrecision = (int)Category::MaxDecimalPrecision(fileLevel);
    uint decimalPrec = (recommendedPrecision < (int)VCardT::decimal) ? (uint)recommendedPrecision
                                                                     : (uint)VCardT::decimal;

    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    decimalPrec);

    double* v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = indices[i];
    }

    ++report.data.columnIndex;
}

template<uint Size, class VCardT>
inline void MinMaxBase::InternalExportValues(SurveyResults& report, const double* values) const
{
    assert(values);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (isInf_ ? "min" : "max");
    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    VCardT::decimal);

    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    double* v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = values[i];
    }

    ++report.data.columnIndex;
}

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_MIN_HXX__
