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
#include "timestamp.h"
#include <time.h>
#include <cassert>
#include "../core/system/gettimeofday.h"

namespace Yuni::DateTime
{
Timestamp Now()
{
#ifdef YUNI_OS_MSVC
    return (int64_t)::_time64(nullptr);
#else
    return (int64_t)::time(nullptr);
#endif
}

Timestamp NowMilliSeconds()
{
    timeval now;
    YUNI_SYSTEM_GETTIMEOFDAY(&now, nullptr);
    return now.tv_sec * 1000 + now.tv_usec / 1000;
}

} // namespace Yuni::DateTime

namespace Yuni::Private::DateTime
{
namespace // anonymous
{
static inline uint FormatString(char* buffer, uint size, const char* format, int64_t timestamp)
{
    assert(format != nullptr and '\0' != *format and "invalid format");

    uint written = 0;

    // Note that unlike on (all?) POSIX systems, in the Microsoft
    // C library localtime() and gmtime() are multi-thread-safe, as the
    // returned pointer points to a thread-local variable. So there is no
    // need for localtime_r() and gmtime_r().

    // \note The variable stdtimestamp is used to ensure the compilation on
    //  32bits platforms

#ifdef YUNI_OS_MINGW
    {
        // MinGW
        time_t stdtimestamp = (time_t)timestamp;
        written = (uint)::strftime(buffer, size, format, ::localtime(&stdtimestamp));
    }
#else
    {
        struct tm timeinfo;

#ifdef YUNI_OS_MSVC
        {
            // Microsoft Visual Studio
            _localtime64_s(&timeinfo, &timestamp);
            written = (uint)::strftime(buffer, size, format, &timeinfo);
        }
#else
        {
            // Unixes
            time_t stdtimestamp = (time_t)timestamp;
            ::localtime_r(&stdtimestamp, &timeinfo);
            written = (uint)::strftime(buffer, size, format, &timeinfo);
        }
#endif
    }
#endif

    return (written and written < size) ? written : 0;
}

} // anonymous namespace

char* FormatTimestampToString(const AnyString& format, int64_t timestamp)
{
    assert(not format.empty() and "this routine must not be called if the format is empty");

    if (timestamp <= 0)
    {
#ifdef YUNI_OS_MSVC
        timestamp = (int64_t)::_time64(nullptr);
#else
        timestamp = (int64_t)::time(nullptr);
#endif
    }

    // trying to guess the future size of the formatted string to reduce memory allocation
    uint size = format.size();
    // valgrind / assert...
    assert(format.c_str()[format.size()] == '\0' and "format must be zero-terminated");
    size += 128; // arbitrary value

    char* buffer = nullptr;
    uint tick = 10;
    do
    {
        buffer = (char*)::realloc(buffer, size * sizeof(char));
        if (FormatString(buffer, size, format.c_str(), timestamp))
        {
            return buffer;
        }

        // there was not enough room for storing the formatted string
        // trying again with more rooms
        size += 256;
    } while (0 != --tick);

    ::free(buffer);
    return nullptr;
}

} // namespace Yuni::Private::DateTime
