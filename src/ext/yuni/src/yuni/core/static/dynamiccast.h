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
#include "inherit.h"

namespace Yuni::Static
{
// Forward declaration
template<class C, class BaseT, int CInheritsFromBaseT = InheritsFrom<C, BaseT>::Yes>
struct DynamicCastWhenInherits;

/*!
** \brief Perform dynamic_cast operations only when a class `C` inherits from another one `BaseT`
**
** \tparam C Any class (may be virtual)
** \tparam BaseT The base class to compare with (may be virtual)
*/
template<class C, class BaseT, int CInheritsFromBaseT>
struct DynamicCastWhenInherits
{
    enum
    {
        Yes = 1,
        No = 0,
    };

    /*!
    ** \brief Test if two pointer are equivalent (means "represents the same real object")
    **
    ** \param ref A pointer to a base class
    ** \param objPointer A pointer to any descendant of `ref`
    ** \return True if C inherits from BaseT and that the two pointer are dynamically equal
    **   (represent the same object)
    */
    template<class U, class V>
    static bool Equals(const U* ref, const V* objPointer)
    {
        return ref == dynamic_cast<const U*>(objPointer);
    }

    template<class U>
    static BaseT* Perform(U* object)
    {
        return dynamic_cast<BaseT*>(object);
    }

    template<class U>
    static const BaseT* PerformConst(const U* object)
    {
        return dynamic_cast<const BaseT*>(object);
    }
};

template<class C, class BaseT>
struct DynamicCastWhenInherits<C, BaseT, 0>
{
    enum
    {
        Yes = 0,
        No = 1,
    };

    template<class U, class V>
    static bool Equals(const U*, const V*)
    {
        return false;
    }

    template<class U>
    static BaseT* Perform(U*)
    {
        return NULL;
    }

    template<class U>
    static const BaseT* PerformConst(const U*)
    {
        return NULL;
    }
};

} // namespace Yuni::Static
