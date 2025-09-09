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
#include "../../yuni.h"

#ifdef YUNI_OS_WINDOWS
#include "username.h"
#include "windows.hdr.h"

namespace Yuni::Private::System
{
uint WindowsUsername(char* cstring, uint size)
{
    enum
    {
        // The maximum length, (see UCLEN)
        defaultSize = 256,
    };

    DWORD unwsize = defaultSize;

    wchar_t unw[defaultSize];
    if (GetUserNameW(unw, &unwsize))
    {
        if (unwsize > 0)
        {
            // The variable `unwsize` contains the final zero
            --unwsize;
            // Getting the size of the buffer into UTF8
            int sizeRequired = WideCharToMultiByte(CP_UTF8,
                                                   0,
                                                   unw,
                                                   unwsize,
                                                   nullptr,
                                                   0,
                                                   nullptr,
                                                   nullptr);
            if (sizeRequired > 0)
            {
                if (static_cast<uint>(sizeRequired) > size)
                {
                    sizeRequired = size;
                }
                WideCharToMultiByte(CP_UTF8,
                                    0,
                                    unw,
                                    unwsize,
                                    cstring,
                                    sizeRequired,
                                    nullptr,
                                    nullptr);
                return static_cast<uint>(sizeRequired);
            }
        }
    }
    return 0;
}

} // namespace Yuni::Private::System

// namespace Yuni

#endif
