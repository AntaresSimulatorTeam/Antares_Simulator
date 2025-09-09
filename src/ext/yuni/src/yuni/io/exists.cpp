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
#include "io.h"
#include "../core/system/windows.hdr.h"
#include "../core/string.h"
#include "../core/string/wstring.h"
#include <sys/stat.h>

#ifndef YUNI_OS_WINDOWS
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef YUNI_HAS_STRING_H
#include <string.h>
#endif

namespace Yuni::IO
{
namespace // anonymous
{
static inline NodeType Stat(const AnyString& filename,
                            uint64_t& outSize,
                            int64_t& lastModified,
                            bool followSymLink)
{
    assert(not filename.empty());

#ifdef YUNI_OS_WINDOWS
    {
        (void)followSymLink; // unused on windows

        const char* p = filename.c_str();
        AnyString::Size len = filename.size();

        // remove the final slash if any
        if (p[len - 1] == '\\' or p[len - 1] == '/')
        {
            if (0 == --len)
            {
                return Yuni::IO::typeUnknown;
            }
        }

        // Driver letters
        if (len == 2 and p[1] == ':' and String::IsAlpha(p[0]))
        {
            return Yuni::IO::typeFolder;
        }

        String norm;
        Yuni::IO::Normalize(norm, AnyString(p, len));

        // Conversion into wchar_t
        WString wstr(norm, true);
        if (not wstr.empty())
        {
            WIN32_FILE_ATTRIBUTE_DATA infoFile;
            if (0 != GetFileAttributesExW(wstr.c_str(), GetFileExInfoStandard, &infoFile))
            {
                LARGE_INTEGER size;
                size.HighPart = infoFile.nFileSizeHigh;
                size.LowPart = infoFile.nFileSizeLow;
                outSize = size.QuadPart;
                lastModified = Yuni::Windows::FILETIMEToTimestamp(infoFile.ftLastWriteTime);

                return ((infoFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                         ? Yuni::IO::typeFolder
                         : Yuni::IO::typeFile;
            }
        }
    }
#else // WINDOWS
    {
        struct stat s;
        int ret = (followSymLink) ? stat(filename.c_str(), &s) : lstat(filename.c_str(), &s);
        if (0 == ret)
        {
            lastModified = s.st_mtime;
            outSize = (uint64_t)s.st_size;

            if (0 != S_ISREG(s.st_mode))
            {
                return Yuni::IO::typeFile;
            }
            if (0 != S_ISDIR(s.st_mode))
            {
                return Yuni::IO::typeFolder;
            }
            if (0 != S_ISLNK(s.st_mode))
            {
                return Yuni::IO::typeSymlink;
            }
            if (0 != S_ISSOCK(s.st_mode))
            {
                return Yuni::IO::typeSocket;
            }

            return Yuni::IO::typeSpecial;
        }
    }
#endif

    return Yuni::IO::typeUnknown;
}

} // anonymous namespace

NodeType TypeOf(const AnyString& filename, bool followSymLink)
{
    uint64_t size; // useless
    int64_t lastModified;
    return (YUNI_LIKELY(not filename.empty())) ? Stat(filename, size, lastModified, followSymLink)
                                               : IO::typeUnknown;
}

NodeType FetchFileStatus(const AnyString& filename,
                         uint64_t& size,
                         int64_t& lastModified,
                         bool followSymLink)
{
    size = 0u;
    lastModified = 0;
    return (YUNI_LIKELY(not filename.empty())) ? Stat(filename, size, lastModified, followSymLink)
                                               : IO::typeUnknown;
}

} // namespace Yuni::IO
