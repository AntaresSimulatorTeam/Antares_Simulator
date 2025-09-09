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
#include "../../traits/length.h"

namespace Yuni::Extension::CString
{
template<class CStringT, class StringT>
class Fill final
{
public:
    static void Perform(char* data, typename CStringT::Size size, const StringT& pattern)
    {
        const uint patternSize = Traits::Length<StringT, uint>::Value(pattern);
        if (0 == patternSize)
        {
            return;
        }

        const char* const cstr = Traits::CString<StringT>::Perform(pattern);
        // If equals to 1, it is merely a single char
        if (1 == patternSize)
        {
            for (typename CStringT::Size i = 0; i < size; ++i)
            {
                data[i] = *cstr;
            }
            return;
        }
        // We have to copy N times the pattern
        typename CStringT::Size p = 0;
        while (p + patternSize <= size)
        {
            YUNI_MEMCPY(data + p, patternSize * sizeof(char), cstr, patternSize * sizeof(char));
            p += patternSize;
        }
        for (; p < size; ++p)
        {
            data[p] = ' ';
        }
    }
};

template<class CStringT>
class Fill<CStringT, char> final
{
public:
    static void Perform(char* data, typename CStringT::Size size, const char rhs)
    {
        for (typename CStringT::Size i = 0; i != size; ++i)
        {
            data[i] = rhs;
        }
    }
};

} // namespace Yuni::Extension::CString
