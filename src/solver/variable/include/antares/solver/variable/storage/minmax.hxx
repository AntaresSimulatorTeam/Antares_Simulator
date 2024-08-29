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
#ifndef __SOLVER_VARIABLE_STORAGE_MIN_HXX__
#define __SOLVER_VARIABLE_STORAGE_MIN_HXX__

#include <limits>

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
template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::initializeFromStudy(Data::Study& study)
{
    // Next
    NextType::initializeFromStudy(study);
}

template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::reset()
{
    // Reset at the begining of the simulation
    if (OpInferior)
    {
        minmax.resetInf();
    }
    else
    {
        minmax.resetSup();
    }
    // Next
    NextType::reset();
}

template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::merge(uint year, const IntermediateValues& rhs)
{
    if (OpInferior)
    {
        minmax.mergeInf(year, rhs);
    }
    else
    {
        minmax.mergeSup(year, rhs);
    }
    // Next
    NextType::merge(year, rhs);
}

template<bool OpInferior, class NextT>
template<uint Size, class VCardT>
void MinMaxBase<OpInferior, NextT>::InternalExportIndices(SurveyResults& report,
                                                          const MinMaxData::Data* array,
                                                          int fileLevel)
{
    assert(array);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    // Caption
    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (OpInferior ? "min" : "max");

    // Precision
    int recommendedPrecision = (int)Category::MaxDecimalPrecision(fileLevel);
    uint decimalPrec = (recommendedPrecision < (int)VCardT::decimal) ? (uint)recommendedPrecision
                                                                     : (uint)VCardT::decimal;

    // Non applicability
    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    decimalPrec);

    // Values
    auto& v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = (double)array[i].indice;
    }

    // Next column index
    ++report.data.columnIndex;
}

template<bool OpInferior, class NextT>
template<uint Size, class VCardT>
inline void MinMaxBase<OpInferior, NextT>::InternalExportValues(SurveyResults& report,
                                                                const MinMaxData::Data* array)
{
    assert(array);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    // Caption
    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (OpInferior ? "min" : "max");
    // Precision
    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    VCardT::decimal);

    // Non applicability
    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    // Values
    auto& v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = array[i].value;
    }

    // Next column index
    ++report.data.columnIndex;
}

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_MIN_HXX__
