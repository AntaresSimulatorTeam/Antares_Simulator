// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_PRINT_H__
#define __SOLVER_VARIABLE_PRINT_H__

#include <yuni/yuni.h>

namespace Antares::Solver::Variable
{
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

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_PRINT_H__
