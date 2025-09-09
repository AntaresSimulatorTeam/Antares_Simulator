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

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include <cassert>

namespace Yuni::Math
{
template<class T>
inline T Log(T x)
{
    return ::log(x);
}

template<>
inline float Log<float>(float x)
{
    return ::logf(x);
}

template<>
inline long double Log<long double>(long double x)
{
    return ::logl(x);
}

template<class T>
inline T Log2(T x)
{
#if defined(YUNI_OS_MSVC)
    return static_cast<T>(log(x) / YUNI_LOG_2);
#else
    return ::log2(x);
#endif
}

template<>
inline float Log2<float>(float x)
{
#if defined(YUNI_OS_MSVC)
    return static_cast<float>(logf(x) / YUNI_LOG_2);
#else
    return ::log2f(x);
#endif
}

template<>
inline long double Log2<long double>(long double x)
{
#if defined(YUNI_OS_MSVC)
    return static_cast<long double>(logl(x) / YUNI_LOG_2);
#else
    return ::log2l(x);
#endif
}

template<class T>
inline T LogOnePlusX(T x)
{
    /* Assert */
    assert(x > -1. && "x must be greater than -1.0");

    return (Abs(x) >= 1e-4)
             // Direct calculation for larger values
             ? Log(1. + x)
             // Taylor approx. log(1 + x) = x - x^2/2! + x^3/3!...
             : (-0.5 * x + 1.) * x;
}

} // namespace Yuni::Math
