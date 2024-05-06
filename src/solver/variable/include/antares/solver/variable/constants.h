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
#ifndef __SOLVER_VARIABLE_CONSTANT_H__
#define __SOLVER_VARIABLE_CONSTANT_H__

namespace Antares
{
namespace Solver
{
namespace Variable
{
enum Constant
{
    maxHoursInAYear = 8785,
    maxDaysInAYear = 7 * 53 + 1, // 366,
    maxWeeksInAYear = 53,
    maxHoursInADay = 24,
    maxMonths = 12,
    maxDaysInAWeek = 7,
    maxHoursInAWeek = 24 * 7, // 168,
};

template<int I>
struct PrecisionToPrintfFormat
{
    static const char* Value()
    {
        return "%.6f";
    }
};

template<>
struct PrecisionToPrintfFormat<0>
{
    static const char* Value()
    {
        return "%.0f";
    }
};

template<>
struct PrecisionToPrintfFormat<1>
{
    static const char* Value()
    {
        return "%.1f";
    }
};

template<>
struct PrecisionToPrintfFormat<2>
{
    static const char* Value()
    {
        return "%.2f";
    }
};

template<>
struct PrecisionToPrintfFormat<3>
{
    static const char* Value()
    {
        return "%.3f";
    }
};

template<>
struct PrecisionToPrintfFormat<4>
{
    static const char* Value()
    {
        return "%.4f";
    }
};

template<>
struct PrecisionToPrintfFormat<5>
{
    static const char* Value()
    {
        return "%.5f";
    }
};

template<class StringT>
static inline void AssignPrecisionToPrintfFormat(StringT& out, uint precision)
{
    switch (precision)
    {
    case 0:
        out.assign("%.0f", 4);
        break;
    case 1:
        out.assign("%.1f", 4);
        break;
    case 2:
        out.assign("%.2f", 4);
        break;
    case 3:
        out.assign("%.3f", 4);
        break;
    case 4:
        out.assign("%.4f", 4);
        break;
    case 5:
        out.assign("%.5f", 4);
        break;
    default:
        out.assign("%.6f", 4);
        break;
    }
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_CONSTANT_H__
