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
#include "openmode.h"

namespace Yuni::IO::OpenMode
{
const char* ToCString(int mode)
{
    if (mode == OpenMode::append)
    {
        // Shortcut for write|append
        return "ab";
    }
    if (mode == OpenMode::truncate)
    {
        // Shortcut for write|append
        return "wb";
    }

    if (mode & OpenMode::write)
    {
        if (0 == (mode & OpenMode::read))
        {
            if (mode & OpenMode::truncate)
            {
                return "wb";
            }
            if (mode & OpenMode::append)
            {
                return "ab";
            }
        }
        else
        {
            if (mode & OpenMode::truncate)
            {
                return "w+b";
            }
            if (mode & OpenMode::append)
            {
                return "a+b";
            }
            return "r+b";
        }
        return "wb";
    }
    return (mode & OpenMode::read) ? "rb" : "";
}

const wchar_t* ToWCString(int mode)
{
    if (mode == OpenMode::append)
    {
        // Shortcut for write|append
        return L"ab";
    }
    if (mode == OpenMode::truncate)
    {
        // Shortcut for write|append
        return L"wb";
    }

    if (mode & OpenMode::write)
    {
        if (!(mode & OpenMode::read))
        {
            if (mode & OpenMode::truncate)
            {
                return L"wb";
            }
            if (mode & OpenMode::append)
            {
                return L"ab";
            }
        }
        else
        {
            if (mode & OpenMode::truncate)
            {
                return L"w+b";
            }
            if (mode & OpenMode::append)
            {
                return L"a+b";
            }
            return L"r+b";
        }
        return L"wb";
    }
    return (mode & OpenMode::read) ? L"rb" : L"";
}

} // namespace Yuni::IO::OpenMode
