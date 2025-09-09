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
#include "../../system/windows.hdr.h"
#include <time.h>
#ifdef YUNI_OS_WINDOWS
#include <wchar.h>
#endif
#include "time.h"

namespace Yuni::Private::LogsDecorator
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
    ::strftime(buffer, 32, "%F %T", ::localtime(&rawtime));
#endif
}

} // namespace Yuni::Private::LogsDecorator
