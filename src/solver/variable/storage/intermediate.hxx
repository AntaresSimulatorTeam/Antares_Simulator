/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#ifndef __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__
#define __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
inline IntermediateValues::~IntermediateValues()
{
    Antares::Memory::Release(hour);
}

inline void IntermediateValues::reset()
{
    Antares::Memory::Zero(maxHoursInAYear, hour);
    memset(month, 0, sizeof(month));
    memset(week, 0, sizeof(week));
    memset(day, 0, sizeof(day));
}

inline IntermediateValues::Type& IntermediateValues::operator[](const unsigned int index)
{
    return hour[index];
}

inline const IntermediateValues::Type& IntermediateValues::operator[](
  const unsigned int index) const
{
    return hour[index];
}

inline uint64_t IntermediateValues::MemoryUsage()
{
    return +sizeof(Type) * maxHoursInAYear;
}

template<class VCardT>
inline void IntermediateValues::buildAnnualSurveyReport(SurveyResults& report,
                                                        int fileLevel,
                                                        int precision) const
{
    if (!(fileLevel & Category::id))
    {
        switch (precision)
        {
            case Category::hourly:
                internalExportAnnualValues<maxHoursInAYear, VCardT>(report, hour, false);
                break;
            case Category::daily:
                internalExportAnnualValues<maxDaysInAYear, VCardT>(report, day, false);
                break;
            case Category::weekly:
                internalExportAnnualValues<maxWeeksInAYear, VCardT>(report, week, false);
                break;
            case Category::monthly:
                internalExportAnnualValues<maxMonths, VCardT>(report, month, false);
                break;
            case Category::annual:
                internalExportAnnualValues<1, VCardT>(report, &year, true);
                break;
        }
    }
}

template<unsigned int Size, class VCardT, class A>
void IntermediateValues::internalExportAnnualValues(SurveyResults& report,
                                                    const A& array,
                                                    bool annual) const
{
    using namespace Yuni;
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    // Caption
    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = nullptr;
    // Precision
    report.precision[report.data.columnIndex] = PrecisionToPrintfFormat<VCardT::decimal>::Value();
    // Non applicability
    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    // Values
    if (not annual)
    {
        (void)::memcpy(report.values[report.data.columnIndex], array, sizeof(double) * Size);
    }
    else
    {
        double& target = *(report.values[report.data.columnIndex]);
        target = year;
    }

    // Next column index
    ++report.data.columnIndex;
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_HXX__
