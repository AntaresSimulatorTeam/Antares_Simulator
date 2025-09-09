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

namespace Yuni::Functional
{
//! A view is a virtual representation of a collection
template<class T, bool UserTypeT = true>
struct View;

} // namespace Yuni::Functional

#include "view.hxx"

namespace Yuni
{
/*!
** \brief Create a view on a collection
** \ingroup Functional
**
** \code
** uint maxValue = makeView(list).max();
** \endcode
*/
template<class CollectionT>
YUNI_DECL Functional::View<CollectionT> makeView(const CollectionT& collection);

//! Create a view on a collection
template<class CollectionT>
Functional::View<CollectionT> makeView(const CollectionT& collection)
{
    return Functional::View<CollectionT>(collection);
}

//! Create a view on an array slice
template<class T, int N>
Functional::View<T[N]> makeView(uint start, uint end, const T collection[N])
{
    return Functional::View<T[N]>(collection);
}

//! Create a view from iterators
template<class B, class E>
Functional::View<Functional::LoopIterator<B, E>, false> makeView(B begin, E end)
{
    return Functional::View<Functional::LoopIterator<B, E>, false>(begin, end);
}

} // namespace Yuni
