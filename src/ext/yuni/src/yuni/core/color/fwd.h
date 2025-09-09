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
#include "../math.h"
#include "../string.h"
#include "../static/if.h"
#include "../static/types.h"

namespace Yuni::Color
{
// forward declarations
template<class T>
class RGB;
template<class T>
class RGBA;

} // namespace Yuni::Color

namespace Yuni::Private::Color
{
/*!
** \brief A default transparency upperBound, according to a given type
*/
template<class T>
struct DefaultValues
{
    enum
    {
        lowerBound = 0,
        upperBound = 255
    };
};

template<>
struct DefaultValues<float>
{
    enum
    {
        lowerBound = 0,
        upperBound = 1
    };
};

template<>
struct DefaultValues<double>
{
    enum
    {
        lowerBound = 0,
        upperBound = 1
    };
};
#ifdef YUNI_HAS_LONG_DOUBLE
template<>
struct DefaultValues<long double>
{
    enum
    {
        lowerBound = 0,
        upperBound = 1
    };
};
#endif

/*!
** \brief The most appropriate type for printing
*/
template<class T>
struct Print
{
    typedef T Type;
};

template<>
struct Print<char>
{
    typedef int Type;
};

template<>
struct Print<unsigned char>
{
    typedef uint Type;
};

template<class T>
struct SignedType
{
    typedef typename Static::If<(T(-1) < T(0)), int, uint>::Type Type;
};

template<>
struct SignedType<float>
{
    typedef float Type;
};

template<>
struct SignedType<double>
{
    typedef double Type;
};
#ifdef YUNI_HAS_LONG_DOUBLE
template<>
struct SignedType<long double>
{
    typedef long double Type;
};
#endif

/*!
** \brief The most appropriate type for calculation
*/
template<class T, class V>
struct Calculation
{
    typedef typename Static::If<
      Static::Type::StrictlyEqual<T, long double>::Yes
        || Static::Type::StrictlyEqual<V, long double>::Yes,
      long double,
      typename Static::If<
        Static::Type::StrictlyEqual<T, double>::Yes || Static::Type::StrictlyEqual<V, double>::Yes,
        double,
        typename Static::If<Static::Type::StrictlyEqual<T, float>::Yes
                              || Static::Type::StrictlyEqual<V, float>::Yes,
                            float,
                            typename SignedType<T>::Type>::Type>::Type>::Type Type;

}; // struct Calculation

} // namespace Yuni::Private::Color
