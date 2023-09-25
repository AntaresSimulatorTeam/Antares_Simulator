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
#include "../../system/windows.hdr.h"
#include <time.h>
#ifdef YUNI_OS_WINDOWS
#include <wchar.h>
#endif
#include "time.h"

namespace Yuni
{
namespace Private
{
namespace LogsDecorator
{

void WriteCurrentTimestampToBuffer(char buffer[32])
{
#ifdef YUNI_OS_MSVC
    __time64_t rawtime = ::_time64(nullptr);
#else
    time_t rawtime = ::time(nullptr);
#endif


#if defined(YUNI_OS_MSVC)
    struct tm timeinfo;
    // Microsoft Visual Studio
    _localtime64_s(&timeinfo, &rawtime);
    // MSDN specifies that the buffer length value must be >= 26 for validity.
    strftime(buffer, 32, "%F %T", &timeinfo);

#else
    // Unixes
    ::strftime(buffer, 32, "%F %T",  ::localtime(&rawtime));
#endif
}

} // namespace LogsDecorator
} // namespace Private
} // namespace Yuni
