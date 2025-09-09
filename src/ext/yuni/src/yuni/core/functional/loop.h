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
#include "view.h"

namespace Yuni::Functional
{
template<class CollectionT>
class Loop final
{
public:
    enum
    {
        hasMapping = false
    };
};

//! Generic loop for basic STL collections.
template<class T, class Other, template<class, class> class CollectionT>
class Loop<CollectionT<T, Other>> final
{
public:
    typedef CollectionT<T, Other> CollType;
    typedef T ElementType;

    enum
    {
        hasMapping = false
    };

public:
    Loop(const CollType& collection):
        begin(collection.begin()),
        it(collection.begin()),
        end(collection.end())
    {
    }

    void reset()
    {
        it = begin;
    }

    bool empty() const
    {
        return it == end;
    }

    bool next()
    {
        ++it;
        return it != end;
    }

    // T& current() { return *it; }
    const T& current() const
    {
        return *it;
    }

private:
    const typename CollType::const_iterator begin;
    typename CollType::const_iterator it;
    const typename CollType::const_iterator end;

}; // class Loop

//! Generic loop for T* null-terminated !
template<class T>
class Loop<T*> final
{
public:
    typedef T ElementType;

    enum
    {
        hasMapping = false
    };

public:
    Loop(const T* const& collection):
        start(collection),
        ptr(collection)
    {
    }

    void reset()
    {
        ptr = start;
    }

    bool empty() const
    {
        return 0 == *ptr;
    }

    bool next()
    {
        ++ptr;
        return 0 != *ptr;
    }

    // T& current() { return *ptr; }
    const T& current() const
    {
        return *ptr;
    }

private:
    const T* const start;
    const T* ptr;

}; // class Loop

//! Generic loop for const T* null-terminated !
template<class T>
class Loop<const T*> final
{
public:
    typedef T ElementType;

    enum
    {
        hasMapping = false
    };

public:
    Loop(const T* const& collection):
        start(collection),
        ptr(collection)
    {
    }

    void reset()
    {
        ptr = start;
    }

    bool empty() const
    {
        return 0 == *ptr;
    }

    bool next()
    {
        ++ptr;
        return 0 != *ptr;
    }

    // T& current() { return *ptr; }
    const T& current() const
    {
        return *ptr;
    }

private:
    const T* const& start;
    const T* const& ptr;

}; // class Loop

//! Generic loop for T[N]
template<class T, int N>
class Loop<T[N]> final
{
public:
    typedef T ElementType;

    enum
    {
        hasMapping = false
    };

public:
    Loop(const T collection[N]):
        start(0u),
        i(0u),
        end((uint)-1),
        data(collection)
    {
    }

    Loop(uint startIdx, uint endIdx, const T collection[N]):
        start(startIdx),
        i(startIdx),
        end(endIdx),
        data(collection)
    {
    }

    void reset()
    {
        i = start;
    }

    bool empty() const
    {
        return i >= Math::Min((uint)N, end);
    }

    bool next()
    {
        ++i;
        return i < Math::Min((uint)N, end);
    }

    // T& current() { return data[i]; }
    const T& current() const
    {
        return data[i];
    }

private:
    uint start;
    uint i;
    uint end;
    const T* const data;

}; // class Loop

template<class BeginT, class EndT>
class LoopIterator final
{
public:
    typedef typename BeginT::value_type ElementType;

    enum
    {
        hasMapping = false
    };

public:
    LoopIterator(const BeginT& itBegin, const EndT& itEnd):
        begin(itBegin),
        it(itBegin),
        end(itEnd)
    {
    }

    void reset()
    {
        it = begin;
    }

    bool empty() const
    {
        return it == end;
    }

    bool next()
    {
        ++it;
        return it != end;
    }

    // ElementType& current() { return *it; }
    const ElementType& current() const
    {
        return *it;
    }

private:
    BeginT begin;
    BeginT it;
    EndT end;
};

} // namespace Yuni::Functional
