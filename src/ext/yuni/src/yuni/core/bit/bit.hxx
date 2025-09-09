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
#include "bit.h"

namespace Yuni::Bit
{
inline uint32_t Count(uint32_t i)
{
#ifdef YUNI_HAS_GCC_BUILTIN_POPCOUNT
    return static_cast<uint32_t>(__builtin_popcount(i));
#else
    // variable-precision SWAR algorithm
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
#endif
}

template<class T>
inline uint Count(T data)
{
    uint c = 0;
    while (data)
    {
        c += c & 0x1u;
        data >>= 1;
    }
    return c;
}

inline bool Get(const char* data, uint index)
{
#ifdef YUNI_OS_MSVC
    return (YUNI_BIT_GET(data, index)) ? true : false;
#else
    return YUNI_BIT_GET(data, index);
#endif
}

inline void Set(char* data, uint index)
{
    // data[index >> 3] |= 1 << ((ENDIANESS) ? (7 - (i) & 7) : (i) & 7);
    YUNI_BIT_SET(data, index);
}

inline void Unset(char* data, uint index)
{
    // data[index >> 3] ~= (1 << ((ENDIANESS) ? (7 - (index) & 7) : (index) & 7));
    YUNI_BIT_UNSET(data, index);
}

} // namespace Yuni::Bit
