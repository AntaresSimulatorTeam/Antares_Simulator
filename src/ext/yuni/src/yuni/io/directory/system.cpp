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
#include "system.h"
#include "../../core/system/environment.h"

namespace Yuni::IO::Directory::System
{
namespace // anonymous
{
template<class StringT>
static inline bool TemporaryImpl(StringT& out, bool emptyBefore)
{
    if (emptyBefore)
    {
        out.clear();
    }

#if defined(YUNI_OS_WINDOWS)
    {
        if (not Yuni::System::Environment::Read("TEMP", out, false))
        {
            if (not Yuni::System::Environment::Read("TMP", out, false))
            {
                return false;
            }
        }
    }
#else
    {
        // On UNIXes, the environment variable TMPDIR must be checked
        // first. Unfortunately, It may happen that no env variable is available.
        if (not Yuni::System::Environment::Read("TMPDIR", out, false))
        {
            if (not Yuni::System::Environment::Read("TMP", out, false))
            {
                if (not Yuni::System::Environment::Read("TEMP", out, false))
                {
                    out += "/tmp"; // default
                }
            }
        }
    }
#endif

    return true;
}

template<class StringT>
static inline bool UserHomeImpl(StringT& out, bool emptyBefore)
{
    if (emptyBefore)
    {
        out.clear();
    }

#ifdef YUNI_OS_WINDOWS
    {
        if (not Yuni::System::Environment::Read("HOMEDRIVE", out, false))
        {
            out += "C:"; // C by default
        }
        if (not Yuni::System::Environment::Read("HOMEPATH", out, false))
        {
            out += '\\';
        }
        return true;
    }
#else
    {
        // UNIX
        return Yuni::System::Environment::Read("HOME", out, false);
    }
#endif

    return false; // fallback
}

template<class StringT>
static inline bool FontsImpl(StringT& out, bool emptyBefore)
{
    if (emptyBefore)
    {
        out.clear();
    }

#ifdef YUNI_OS_WINDOWS
    {
        if (not Yuni::System::Environment::Read("WINDIR", out, false))
        {
            out += "C:\\Windows"; // C:\Windows by default
        }
        out += "\\Fonts\\";
    }
#elif defined(YUNI_OS_MACOS)
    {
        out = "/Library/Fonts/";
    }
#else // YUNI_OS_LINUX
    {
        out = "/usr/share/fonts/truetype/";
    }
#endif

    return true;
}

} // anonymous namespace

bool Temporary(String& out, bool emptyBefore)
{
    return TemporaryImpl(out, emptyBefore);
}

bool Temporary(Clob& out, bool emptyBefore)
{
    return TemporaryImpl(out, emptyBefore);
}

bool UserHome(Clob& out, bool emptyBefore)
{
    return UserHomeImpl(out, emptyBefore);
}

bool UserHome(String& out, bool emptyBefore)
{
    return UserHomeImpl(out, emptyBefore);
}

bool Fonts(String& out, bool emptyBefore)
{
    return FontsImpl(out, emptyBefore);
}

bool Fonts(Clob& out, bool emptyBefore)
{
    return FontsImpl(out, emptyBefore);
}

} // namespace Yuni::IO::Directory::System
