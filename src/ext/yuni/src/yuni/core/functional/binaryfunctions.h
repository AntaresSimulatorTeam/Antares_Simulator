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
#include "../../yuni.h"
#include "../math.h"

namespace Yuni::Functional
{
template<class T>
class Max
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 = Math::Max(val1, val2);
        return true;
    }

}; // class Max

template<class T>
class Min
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 = Math::Min(val1, val2);
        return true;
    }

}; // class Min

template<class T>
class Add
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 += val2;
        return true;
    }

}; // class Add

template<class T>
class Sub
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 -= val2;
        return true;
    }

}; // class Sub

template<class T>
class Mul
{
public:
    bool operator()(const T& val1, const T& val2) const
    {
        val1 *= val2;
        return true;
    }

}; // class Mul

template<class T>
class Div
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 /= val2;
        return true;
    }

}; // class Div

template<class T>
class And
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 = val1 && val2;
        return true;
    }

}; // class And

template<class T>
class Or
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 = val1 || val2;
        return true;
    }

}; // class Or

template<class T>
class Xor
{
public:
    bool operator()(T& val1, const T& val2) const
    {
        val1 = val1 ^ val2;
        return true;
    }

}; // class Xor

#define lambda(A, B, EXPR)          \
    struct                          \
    {                               \
        bool operator()(A, B) const \
        {                           \
            EXPR;                   \
        }                           \
    }

} // namespace Yuni::Functional
