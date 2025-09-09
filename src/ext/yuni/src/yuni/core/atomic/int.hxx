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
#include "int.h"

namespace Yuni::Atomic
{
template<int Size, template<class> class TP>
inline Int<Size, TP>::Int():
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue()
#else
    TP<Int<Size, TP>>(),
    pValue()
#endif
{
}

template<int Size, template<class> class TP>
inline Int<Size, TP>::Int(int16_t v):
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue(static_cast<ScalarType>(v))
#else
    TP<Int<Size, TP>>(),
    pValue((ScalarType)v)
#endif
{
}

template<int Size, template<class> class TP>
inline Int<Size, TP>::Int(int32_t v):
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue(static_cast<ScalarType>(v))
#else
    TP<Int<Size, TP>>(),
    pValue((ScalarType)v)
#endif
{
}

template<int Size, template<class> class TP>
inline Int<Size, TP>::Int(int64_t v):
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue(static_cast<ScalarType>(v))
#else
    TP<Int<Size, TP>>(),
    pValue(static_cast<ScalarType>(v))
#endif
{
}

template<int Size, template<class> class TP>
inline Int<Size, TP>::Int(const Int<Size, TP>& v):
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue(static_cast<ScalarType>(v))
#else
    TP<Int<Size, TP>>(),
    pValue((ScalarType)v.pValue)
#endif
{
}

template<int Size, template<class> class TP>
template<int Size2, template<class> class TP2>
inline Int<Size, TP>::Int(const Int<Size2, TP2>& v):
#if YUNI_ATOMIC_MUST_USE_MUTEX != 1
    pValue()
#else
    TP<Int<Size, TP>>(),
    pValue()
#endif
{
    if (threadSafe)
    {
        Private::AtomicImpl::Operator<size, TP>::Increment(*this,
                                                           static_cast<ScalarType>(v.pValue));
    }
    else
    {
        pValue = static_cast<ScalarType>(v.pValue);
    }
}

template<int Size, template<class> class TP>
inline Int<Size, TP>::operator ScalarType() const
{
    return (threadSafe) ? Private::AtomicImpl::Operator<size, TP>::Increment(*this, 0) : (pValue);
}

template<int Size, template<class> class TP>
inline typename Int<Size, TP>::ScalarType Int<Size, TP>::operator++()
{
    return Private::AtomicImpl::Operator<size, TP>::Increment(*this);
}

template<int Size, template<class> class TP>
inline typename Int<Size, TP>::ScalarType Int<Size, TP>::operator--()
{
    return Private::AtomicImpl::Operator<size, TP>::Decrement(*this);
}

template<int Size, template<class> class TP>
inline typename Int<Size, TP>::ScalarType Int<Size, TP>::operator++(int)
{
    return (threadSafe) ? Private::AtomicImpl::Operator<size, TP>::Increment(*this) - 1
                        : (pValue++);
}

template<int Size, template<class> class TP>
inline typename Int<Size, TP>::ScalarType Int<Size, TP>::operator--(int)
{
    return (threadSafe) ? Private::AtomicImpl::Operator<size, TP>::Decrement(*this) + 1
                        : (pValue--);
}

template<int Size, template<class> class TP>
inline bool Int<Size, TP>::operator!() const
{
    return (threadSafe) ? (0 == Private::AtomicImpl::Operator<size, TP>::Increment(*this, 0))
                        : (0 == pValue);
}

template<int Size, template<class> class TP>
inline Int<Size, TP>& Int<Size, TP>::operator=(const ScalarType& v)
{
    if (threadSafe)
    {
        Private::AtomicImpl::Operator<size, TP>::Set(*this, v);
    }
    else
    {
        pValue = v;
    }
    return *this;
}

template<int Size, template<class> class TP>
inline Int<Size, TP>& Int<Size, TP>::operator+=(const ScalarType& v)
{
    if (threadSafe)
    {
        Private::AtomicImpl::Operator<size, TP>::Increment(*this, v);
    }
    else
    {
        pValue += v;
    }
    return *this;
}

template<int Size, template<class> class TP>
inline Int<Size, TP>& Int<Size, TP>::operator-=(const ScalarType& v)
{
    if (threadSafe)
    {
        Private::AtomicImpl::Operator<size, TP>::Decrement(*this, v);
    }
    else
    {
        pValue -= v;
    }
    return *this;
}

template<int Size, template<class> class TP>
inline void Int<Size, TP>::zero()
{
    if (threadSafe)
    {
        Private::AtomicImpl::Operator<size, TP>::Zero(*this);
    }
    else
    {
        pValue = 0;
    }
}

} // namespace Yuni::Atomic
